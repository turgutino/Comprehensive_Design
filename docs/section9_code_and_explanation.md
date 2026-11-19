# Section 9 - C program for the board's Linux (code and explanation)

Files: `evidence\section9_hello\hello_linux.c` (source) and `evidence\section9_hello\hello_linux` (built ELF, SHA-256 `f781f84875733ca127af8b8beed7bcf1babaa696e3c1927e80845b2aea1770e6`, 5251 bytes).

## Idea

Linux on the Loongson 1B (MIPS32, little-endian) runs ordinary ELF programs. A Linux C program normally uses a C library that wraps the kernel's system calls. To keep the example small and independent of any library, the program calls the kernel directly with the MIPS `syscall` instruction (o32 convention): the system call number goes into register `v0`, the arguments into `a0..a2`, and the result comes back in `v0`.

- `write(1, msg, len)` = system call 4004 (o32 base 4000 + 4): prints the text on standard output (the console).
- `exit(0)` = system call 4001 (4000 + 1): ends the program.

## Code

```c
static const char msg[] = "Hello from my own program on Loongson 1B Linux!\n";

static inline int sys_write(int fd, const char *buf, int len)
{
    register int v0 __asm__("$2") = 4004;   /* SYS_write (o32: 4000 + 4) */
    register int a0 __asm__("$4") = fd;
    register const char *a1 __asm__("$5") = buf;
    register int a2 __asm__("$6") = len;
    register int a3 __asm__("$7");
    __asm__ volatile("syscall" : "+r"(v0), "=r"(a3)
                     : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v0;
}

static inline void sys_exit(int code)
{
    register int v0 __asm__("$2") = 4001;   /* SYS_exit (o32: 4000 + 1) */
    register int a0 __asm__("$4") = code;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0) : "memory");
    for (;;) { }
}

void __start(void) __asm__("__start");
void __start(void)
{
    int n = 0;
    while (msg[n]) n++;      /* length of the text */
    sys_write(1, msg, n);
    sys_exit(0);
}
```

## Build (with the SDE Lite 4.9.2 compiler already installed for the bare-metal sections)

```
mips-sde-elf-gcc -EL -march=mips32 -mabi=32 -mno-abicalls -fno-pic -O1 -nostdlib -nostartfiles -static \
  -Wl,-e,__start -Wl,-Ttext=0x00400000 -Wl,--build-id=none -Wl,-z,max-page-size=0x1000 \
  -o hello_linux hello_linux.c
```

`-EL` little-endian, `-mabi=32` o32 calling convention, `-static -nostdlib` no library and no dynamic loader, `-Ttext=0x00400000` usual load address of a MIPS Linux program, `-Wl,-e,__start` entry symbol.

## What was checked on the PC (`readelf`, `objdump`)

- ELF32, little endian, type EXEC, machine MIPS, flags `o32, mips32`, entry point `0x400000`.
- One loadable segment (`LOAD`, offset `0x1000`, address `0x00400000`, flags R E).
- Disassembly: a loop that counts the characters, then `li v0,4004 ... syscall`, then `li v0,4001 ... syscall`.
