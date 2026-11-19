# Request to the instructor - missing materials and guide issues

Course: Computer System Comprehensive Design (Loongson 1B).
Date: 2026-11-02.

## 1. Materials we do not have

| # | Item | Needed for |
|---|------|-----------|
| 1 | `example_7_2` project archive (the guide says it is provided) | RECEIVED 2026-11-03 (see `REPORT.md` section 4); it is a cache experiment, not the odd-number sum. |
| 2 | `loongson_linux_file` folder: `gcc-4.3-ls232.tar.gz`, `linux.zip` (Linux 3.0 for ls1b), `buildroot.tar.gz` | Section 9 (Linux port). Shared via the VM shared folder `D:\loongson_linux_file`. |
| 3 | Serial terminal tool **Alapp-ISP** (the guide shows version 6.95E) | Section 6. ANSWERED 2026-11-08: instructor sent `AiCube-ISP-v6.96M.zip` (not yet downloaded). |
| 4 | Network tool **NetAssist** (shown as V5.0.2 in the guide) | Section 8 |
| 5 | Loongson 1B user manual (GPIO, UART, GMAC register descriptions) | Sections 6 and 8, understanding the drivers |
| 6 | Board, Mini-USB cable, DB9 serial-to-USB cable (UART5), Ethernet cable, 5 V supply | Debugging and Sections 6-9 |
| 7 | Source of the board BSP / driver headers if separate from LoongIDE templates | Reference |

## 2. Questions about the guide

1. **Section 9.2.7 - root file system format.** The text says the build produces `rootfs.ext2` in `output/image`, and it is flashed to `/dev/mtd1`. Figure 9.49 shows the build creating `output/images/rootfs.yaffs2` (YAFFS2 on NAND). Which file must be flashed, and with which command?
2. **Section 9 - network topology.** The text says the board connects directly to the PC with an Ethernet cable. The screenshots show the PC on Wi-Fi (192.168.0.104) with Tftpd32 bound to that address, which implies a router. Which setup is expected?
3. **Section 9.2.5** gives `gcc-4.3-ls232.tar.gz` in the command, but the file list says `gcc-4.3-ls232.tar`. Confirm the real file name.
4. **Section 8.2.3** - in `lwip_test.h` the closing `#endif` of the include guard is placed right after `#define _LWIP_TEST_H`, so all following definitions fall outside the guard. Should it be moved to the end of the file?
5. **Section 8.2.14 / 8.2.15** - the UDP server and client tests list the protocol type as "TCP Server" and "TCP UDP" in NetAssist. We assume it should be "UDP". Please confirm.
6. **Section 8.2.11** - the PC IP address (192.168.1.111) is not listed as a step, only the mask and gateway. Please confirm the intended PC address.
7. **Section 9.2.10** - host IPs are given both as `192.168.0.10` and `192.168.0.104`. We will use the real address from `ipconfig`.
8. **Section 6** - the guide says to "set the UART3 program to false mode". Figure 6.1 shows `.bIntrrupt = false` in the `ls1b_UART3` structure of `ns16550.c` (polled UART). In polled mode `ls1x_uart_read(devUART3, &byte, 1, 1)` blocks until a byte arrives (the last argument is a timeout; non-zero = blocking), so the running-LED loop would advance only when a byte is received. In interrupt mode (the BSP default, `true`) the same call returns after about 1 ms. Which mode is intended?
9. **Section 5** - the assignment says overflow "will cause an exception", but the supplied code uses `addu` with a prior range check, which never raises an overflow exception. Is the intended behaviour only the pre-check?

## 2.1 Questions added 2026-11-08

10. ANSWERED 2026-11-09 (told by the student in the lab): all sections (4-9) must be done.
11. ANSWERED 2026-11-09 (told by the student in the lab): there is no separate "final project"; the course work is the experiments being done.
12. `example_7_2` was described as an example and a "different design" is expected. What kind of own design is required (for instance our own assembly or cache program, or an extension of a guide section)?
13. Section 5: is the required task the odd-number sum, the cache experiment, or both?
14. Cache experiment: after `sw s0, 0(0xA0030000)` the cached load `lw a2, 0(0x80030000)` and a memory view of `0xA0030000` both returned `0xFFFE0001` (the Section 5 result from an earlier run) instead of `0x12345678`. Is a stale value expected here (for example the cache is not invalidated at reset), and is the debugger memory view cached or uncached?
15. `AiCube-ISP-v6.96M` replaces Alapp-ISP 6.95E for Section 6.1 - please confirm.

## 2.2 Questions about Section 6 hardware (added 2026-11-08)

16. Figure 6.2 shows an 8-LED add-on board connected to the 40-pin header and a USB-serial adapter on a DB9 connector. Is the LED board provided in the lab, and which DB9 connector is UART3?
17. Please confirm that the LED pins in the guide code (GPIO 9, 11, 15, 17, 20, 22, 6, 7) match the LED board and that the LEDs are active-low (`gpio_write(pin, 0)` = on). We cannot check this without the board schematic.
18. The guide code uses `strncmp`, `strcmp` and `atoi` but includes only `<stdio.h>`; it compiles with warnings only (implicit declarations). Is adding `<string.h>` and `<stdlib.h>` acceptable?

## 2.3 Questions about Section 9 (added 2026-11-10)

19. The commands in 9.2.10 (`mtd_erase`, `devcp`, `set al`, `set append`) overwrite the Linux that is already in the board's flash. May we run them on our board, and is there a way to restore the original system if something goes wrong?
20. Will you provide `gcc-4.3-ls232.tar(.gz)`, `linux.zip` and `buildroot.tar.gz` (and the Tftpd32 and VMware installers, or is another Linux environment such as WSL acceptable)?
21. Which port is the PMON/Linux console in Section 9: the FTDI virtual port of the Micro-USB cable (guide 9.2.9) or the separate USB-serial cable on UART5 (Section 7)?
22. Which host IP address is intended for the TFTP server (`192.168.0.10`, `192.168.0.104` or another), and should the board be connected directly or through a router?

## 3. Slide question (slide 11)

"What is the difference between RISC and CISC?" - answer prepared in `docs\konspekt_MIPS32.md`, section 3.
