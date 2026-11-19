# Message to the instructor (draft, 2026-11-10)

Hello teacher,

Status of the Loongson 1B practical work, and a few things I need from you.

**Done on the board:** Section 4 (C project, debug), Section 5 (assembly, odd-number sum), your cache example `example_7_2` (ran it, then wrote my own variant `cache_probe` to explain the unexpected result). Sections 6, 7, 8, 9 are prepared and build offline (Section 7 builds with 0 errors) but could not be run yet.

**Blocking problem - serial console.** The USB-serial (CH340) cable is listed by Windows as working, but no program can open the port: "A device attached to the system is not functioning" (PuTTY: "Unable to configure serial port"). I tried other USB ports, a clean driver re-install (your `CH341SER.EXE`), a restart and the device settings. It worked on 2026-11-03 on the same PC. Could I borrow another USB-serial cable, or test mine on another PC?

**What I need for the other sections:**
1. Section 6: the 8-LED add-on board (guide Figure 6.2), which DB9 connector is UART3, and the `AiCube-ISP` tool (I did not receive the file yet).
2. Section 8: an Ethernet cable and the NetAssist tool (or may I use my own Python test scripts on the PC side?).
3. Section 9: the files `gcc-4.3-ls232.tar(.gz)`, `linux.zip`, `buildroot.tar.gz`; are VMware and an Ubuntu 16.04 VM required, or is WSL acceptable?

**Questions:**
4. Section 9 overwrites the board's flash (`mtd_erase`, `devcp`, `set`). May I do this on my board, and how can the original Linux be restored if something fails?
5. Which port is the console in Section 9: the Micro-USB virtual port or the UART5 cable?
6. Section 5: which assignment is required - the odd-number sum (done), the cache experiment (done), or both?
7. Section 7: the guide code prints "barber find customer 1, 2, 3" in Figure 7.14, but the code as printed should print 0, 1, 2. Which is correct for the report?

I keep all results, hashes and screenshots in a lab report and can show them. Thank you.
