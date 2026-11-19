# Study Notes: MIPS32 and Loongson 1B (introductory part of the course)

Sources: `materials\导论.pptx` (77 slides), MIPS32 Volume I/II/III PDFs, the course experiment guide.

## 1. Course and platform
- Course: Computer System Comprehensive Design. It integrates earlier subjects: computer organization, interfaces, assembly, C, data structures, OS, networking.
- Hardware: **Loongson 1B** (SoC, MIPS32 Release 2) development board. The board has UART3/4/5, RJ45, USB, CAN, ADC/DAC, LCD and 8 LEDs.
- Software: **LoongIDE** (GNU toolchain, requires MinGW/MSYS), **RT-Thread** (RTOS), Linux.
- SoC: CPU + memory controller + peripherals on one chip. Three-level structure: XBAR, AXI-MUX, APB (AMBA: AXI, APB).
- Note: Loongson first used MIPS, later its own LoongArch ISA. This course teaches MIPS32.

## 2. Architecture vs. implementation
- **Architecture**: instruction set, registers, exception model, memory management, virtual/physical address layout. Identical on all MIPS32 processors.
- **Implementation**: how a specific processor realizes it (pipeline length, cache type, FPU algorithms).
- MIPS32 = ISA + **PRA** (Privileged Resource Architecture: virtual memory, caches, exceptions, CP0).
- MIPS32 is based on MIPS II, with selected instructions from MIPS III-V. MIPS64 is a superset of MIPS32.

## 3. RISC vs. CISC (question on slide 11)
| | RISC (MIPS, ARM Cortex, RISC-V) | CISC (x86) |
|---|---|---|
| Instructions | Few, simple, **fixed length** (32 bits in MIPS) | Many, complex, **variable length** |
| Memory access | Only via load/store | Arithmetic instructions may operate on memory directly |
| Registers | Many (MIPS: 32) | Few |
| Execution | Pipeline-friendly (about one instruction per cycle) | Complex decoding, split into micro-operations |
| Compiler | Register allocation and scheduling are the compiler's job | Hardware does more of the work |

(This table is general knowledge; the slides do not give the answer.)

## 4. Pipeline
- Stages: Fetch, ALU, Memory, Write back.
- **Simple**: the next instruction does not start until the previous one finishes (1 instruction per 4 cycles).
- **Parallel (pipelined)**: a new instruction starts every cycle; the first result appears after 4 cycles, then one per cycle.
- **Superpipeline**: each stage takes a fraction of a clock cycle (e.g. half).
- **Superscalar**: several instructions per cycle (IF, ID, IS, EX, WB).
- Pipelining and multiple issue belong to the **implementation**, not the architecture.

## 5. Load/Store architecture
- All computation is done in registers. Memory is accessed only with `lw/sw/lb/sb/...`.
- Benefits: fewer memory accesses, simpler ISA, easier register allocation for compilers.

## 6. Registers
- **32 general-purpose 32-bit registers** (r0-r31), **HI, LO** (multiply/divide results), **PC** (not directly visible).
- `r0` is always 0 (also used to discard a result). `r31` is the **return address** for JAL, BLTZAL, BGEZAL, etc.
- HI/LO: after a multiply, the product (LO = low 32 bits, HI = high 32 bits). After a divide, **LO = quotient, HI = remainder**.
- Common naming convention: `zero(0)`, `at(1)`, `v0-v1(2-3)`, `a0-a3(4-7)`, `t0-t7(8-15)`, `s0-s7(16-23)`, `t8-t9(24-25)`, `k0-k1(26-27)`, `gp(28)`, `sp(29)`, `fp(30)`, `ra(31)`.

## 7. Endianness and alignment
- **Big-endian**: byte 0 is the most significant (left) byte. **Little-endian**: byte 0 is the least significant (right) byte.
- In the guide, Loongson 1B is configured **little-endian** (`-EL`).
- Bit numbering always starts at 0 on the right.
- Alignment: halfword on even addresses, word on multiples of 4, doubleword on multiples of 8. Violation raises an **Address Error** exception.
- Unaligned access: the `LWL/LWR/SWL/SWR` pairs.

## 8. Memory map (32-bit virtual address space)
| Segment | Address range | Properties |
|---|---|---|
| useg/kuseg | 0x0000_0000 - 0x7FFF_FFFF | Mapped (TLB), user |
| **kseg0** | 0x8000_0000 - 0x9FFF_FFFF | Kernel, **unmapped, cached**, physical 0 - 0x1FFF_FFFF |
| **kseg1** | 0xA000_0000 - 0xBFFF_FFFF | Kernel, **unmapped, uncached** (used for peripherals) |
| ksseg | 0xC000_0000 - 0xDFFF_FFFF | Supervisor/kernel, mapped |
| kseg3 | 0xE000_0000 - 0xFFFF_FFFF | Kernel, mapped |
- Memory access types: **uncached** (every access goes to physical memory), **cached** (caches are used).
- Our addresses `0x80200000` (program) and `0x80030000` (result) are in kseg0.

## 9. Instruction groups
1. **Load/Store**: LB, LBU, LH, LHU, LW, SB, SH, SW; unaligned LWL/LWR/SWL/SWR; atomic LL/SC.
2. **Computational**: ADD/ADDI/ADDU/ADDIU, SUB/SUBU, AND/OR/XOR/NOR, SLT/SLTU/SLTI/SLTIU, LUI, shifts (SLL, SRL, SRA, ROTR), MULT/MULTU/DIV/DIVU, MFHI/MFLO/MTHI/MTLO, MUL, MADD/MSUB, CLO/CLZ.
3. **Jump/Branch**: J, JAL, JR, JALR; BEQ, BNE, BGEZ, BGTZ, BLEZ, BLTZ (and the "and link" variants).
4. **Miscellaneous**: SYNC/SYNCI, SYSCALL/BREAK, traps (TEQ, TGE, TLT, TNE, ...), MOVZ/MOVN, PREF, NOP/SSNOP.
5. **Coprocessor**: CP0 (system control), CP1 (FPU), CP2 (implementation specific).
- The name "unsigned" is misleading: **ADDU/ADDIU/SUBU never raise an overflow exception** (modulo 2^32 arithmetic). **ADD/ADDI/SUB** raise an Integer Overflow exception.

## 10. Instruction formats (all 32 bits)
- **R-type**: `opcode(6) | rs(5) | rt(5) | rd(5) | sa(5) | function(6)`
- **I-type**: `opcode(6) | rs(5) | rt(5) | immediate(16)`
- **J-type**: `opcode(6) | instr_index(26)`
- The branch offset is 16 bits, shifted left by 2 (range about +/-128 KB). J/JAL work within the current 256 MB region.

## 11. Branch delay slot
- The **one instruction after** every branch/jump is executed before the branch takes effect (the delay slot).
- No branch/jump may be placed in a delay slot.
- **Branch likely** instructions (BEQL, ...) nullify the delay slot when the branch is not taken. They are deprecated and should not be used.
- `.set reorder` tells the assembler to fill delay slots itself (with nop). Pseudo-instructions such as `bltu` expand to `sltu` + `bnez`.

## 12. CP0 and exceptions
- CP0 translates virtual to physical addresses and controls exceptions, caches and modes. Key registers: **Status** (IE, EXL, ERL, BEV), **Cause** (ExcCode, IP), **EPC** (return address), **BadVAddr**, **Count/Compare** (timer), **Config**, **PRId**.
- Reset vector: `0xBFC0_0000`. General exception vector: `0x8000_0180` (with `BEV=0`); interrupts may use a dedicated vector.
- Example ExcCodes: 0 Int, 4 AdEL, 5 AdES, 8 Sys, 9 Bp, 10 RI, 12 Ov, 13 Tr.
- `ERET` returns from an exception (it has no delay slot).
- Operating modes: Kernel, Supervisor (optional), User.

## 13. Link to the course examples
- `example_4_1` (C): `x=a+b; y=a-b;` compiled to `lbu`, `addu`, `sll/sra 24` (sign extension to 8 bits) and `sb`. `main` is at `0x802000F4`.
- Section 5 (assembly): the sum `1+3+5+...` equals `n^2`. The maximum n is 65535, the sum is `0xFFFE0001`, and the last odd number added is `0x1FFFD`. Overflow is avoided with `addu` plus a prior check (`subu`, `bltu`), so no exception is raised.
