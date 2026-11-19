/* Section 9 (own variant): minimal static Linux program for MIPS32 little-endian (o32).
 * No libc: raw Linux system calls (write = 4004, exit = 4001), so that the bare-metal
 * toolchain mips-sde-elf-gcc (SDE Lite 4.9.2) is enough to build it.
 * Runs on the board's existing Buildroot Linux; nothing is written to flash.
 */
static const char msg[] = "Hello from my own program on Loongson 1B Linux!\n";

static inline int sys_write(int fd, const char *buf, int len)
{
    register int v0 __asm__("$2") = 4004;   /* SYS_write (o32: 4000 + 4) */
    register int a0 __asm__("$4") = fd;
    register const char *a1 __asm__("$5") = buf;
    register int a2 __asm__("$6") = len;
    register int a3 __asm__("$7");
    __asm__ volatile("syscall"
                     : "+r"(v0), "=r"(a3)
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
    while (msg[n]) n++;
    sys_write(1, msg, n);
    sys_exit(0);
}
