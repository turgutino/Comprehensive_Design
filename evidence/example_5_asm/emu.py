"""Minimal MIPS32 emulator that executes the real machine code of bsp_start
taken from the linked image, to verify the Section 5 result independently
of objdump and of the (unavailable) board."""
import struct, sys

TEXT_BASE = 0x80200000
code = open("text.bin", "rb").read()

def fetch(addr):
    off = addr - TEXT_BASE
    return struct.unpack_from("<I", code, off)[0]

def s16(x): return x - 0x10000 if x & 0x8000 else x
M = 0xFFFFFFFF
reg = [0] * 32
mem = {}
HI = LO = 0
T = dict(zero=0, at=1, v0=2, v1=3, a0=4, a1=5, a2=6, a3=7, t0=8, t1=9, t2=10, t3=11,
         t4=12, t5=13, t6=14, t7=15, s0=16, s1=17, s2=18, s3=19, s4=20, s5=21, s6=22,
         s7=23, t8=24, t9=25, k0=26, k1=27, gp=28, sp=29, fp=30, ra=31)
SENTINEL = 0xDEAD0000
reg[31] = SENTINEL
pc = int(sys.argv[1], 16)
steps = 0
npc_override = None

def step_one(pc):
    """Execute instruction at pc; return (next_pc_if_branch_taken or None)."""
    global HI, LO
    w = fetch(pc)
    op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; rd = (w >> 11) & 31
    sa = (w >> 6) & 31; fn = w & 63; imm = w & 0xFFFF
    target = None
    if op == 0:
        if fn == 0:   reg[rd] = (reg[rt] << sa) & M          # sll / nop
        elif fn == 0x21: reg[rd] = (reg[rs] + reg[rt]) & M   # addu
        elif fn == 0x23: reg[rd] = (reg[rs] - reg[rt]) & M   # subu
        elif fn == 0x2B: reg[rd] = 1 if reg[rs] < reg[rt] else 0   # sltu
        elif fn == 0x25: reg[rd] = reg[rs] | reg[rt]         # or
        elif fn == 8: target = reg[rs]                       # jr
        else: raise SystemExit(f"unsupported SPECIAL fn=0x{fn:x} at {pc:08x}")
    elif op == 9:  reg[rt] = (reg[rs] + s16(imm)) & M        # addiu
    elif op == 0xF: reg[rt] = (imm << 16) & M                # lui
    elif op == 0xD: reg[rt] = reg[rs] | imm                  # ori
    elif op == 0x2B:                                          # sw
        a = (reg[rs] + s16(imm)) & M
        assert a % 4 == 0, "unaligned sw"
        mem[a] = reg[rt]
    elif op == 4:
        if reg[rs] == reg[rt]: target = (pc + 4 + (s16(imm) << 2)) & M   # beq
    elif op == 5:
        if reg[rs] != reg[rt]: target = (pc + 4 + (s16(imm) << 2)) & M   # bne
    else:
        raise SystemExit(f"unsupported opcode 0x{op:x} at {pc:08x}")
    reg[0] = 0
    return target, (op in (4, 5) or (op == 0 and fn == 8))

while pc != SENTINEL:
    target, is_branch = step_one(pc)
    steps += 1
    if is_branch:                      # execute delay slot, then branch
        step_one(pc + 4)
        steps += 1
        pc = target if target is not None else pc + 8
    else:
        pc += 4
    if steps > 10_000_000: raise SystemExit("runaway")

n = lambda r: f"{reg[T[r]]:#010x}"
print("steps executed :", steps)
for r in ("t0", "t1", "t2", "t3", "t4", "t5", "s0"):
    print(f"{r} = {n(r)}  ({reg[T[r]]})")
print("mem[0x80030000] =", f"{mem.get(0x80030000, None):#010x}", "->", mem.get(0x80030000))
# independent checks
exp_n = 65535
assert reg[T['t0']] == exp_n * exp_n == 4294836225
assert reg[T['t3']] == 2 * exp_n - 1 == 131069
assert reg[T['t1']] == 2 * exp_n + 1 == 131071
assert mem[0x80030000] == 0xFFFE0001
assert reg[T['t0']] + reg[T['t4']] == 0xFFFFFFFF
print("ALL CHECKS PASSED (sum = 65535^2 = 0xFFFE0001, last added = 131069, next = 131071)")
