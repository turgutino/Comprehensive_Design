# Project status

Last updated: 2026-11-08

## Update 2026-11-10 (15:20): board Linux does not boot (kernel panic)

- Since about 15:17 the board stops with `Kernel panic - not syncing: Attempted to kill init` (`/lib/libm.so.6: cannot read file data`). Three times in a row, also with debug and Ethernet cables removed. Cause unknown; no flash command was ever used. Details and the instructor message: `docs\INCIDENT_linux_rootfs_2026-11-10.md`.
- Section 9 (own variant): the static Linux test program `evidence\section9_hello\hello_linux` is built (MIPS32 EL, o32, 5251 bytes) but could not be run: no working Linux on the board. Needs the instructor (rootfs restore needs approval).
- Do not experiment further with the flash; wait for the instructor.

## Update 2026-11-10 (late afternoon): Section 6 RUN ON THE BOARD (mostly)

- LED module wired to Table 8.14 (GPIO 9, 11, 15, 17, 20, 22, 6, 7), `example_6_led` rebuilt (exe `f2bb2327...`), serial control through UART3 with AiCube-ISP's serial helper on COM7. Board echoes commands; `ON 6` lit one LED (photo). Details: `REPORT.md` 5.2.1, `evidence\example_6_led\`.
- Not documented: `START` / `SPEED` / `STOP` (student reports they work, no screenshot/video); LED silkscreen numbering; polled-mode check. Debug start kept printing `wait for pracc ... fail` but the program ran.

## Update 2026-11-10 (evening): Section 8 DONE ON THE BOARD

- Tests 1-3 (TCP server, TCP client, UDP server) passed cleanly; test 4 (UDP client) works but the first datagram after a board boot is lost on the PC side (analysed with pktmon, cause not determined); finished with the `--nudge` workaround. Details and evidence: `REPORT.md` 5.4, `evidence\example_8_lwip\`.
- Firewall clean-up done and verified; the Ethernet adapter was set back to DHCP by the student on 2026-11-10 (verified). A static `192.168.1.111/24` is needed again for any further Section 8 run (see `docs\SYSTEM_CHANGES.md`).
- Still open: Section 6 (LED board, UART3 cable, AiCube), Section 9 (instructor files, flash permission), instructor answers, final hand-in document (docx/PDF).

## Update 2026-11-10 (later): Section 7 DONE ON THE BOARD

- The CH340 serial adapter opened `COM7` after pressing `INSTALL` in the instructor's `CH341SER.EXE` with the device plugged in (cause not proven; earlier attempts are in `REPORT.md` 1.3.1).
- `RThread6` ran on the board; console output captured in `evidence\example_7_barber\putty_COM7_20261110_105614.log`: 3 haircuts, 2 customers leave; barber counter prints 0, 1, 2 as predicted (guide figure shows 1, 2, 3). Details in `REPORT.md` 5.3.
- Ethernet link is up on the PC (100 Mbps, no static IP yet). Next: Section 8 (PC address 192.168.1.111/24, project `example_8_lwip`, Python peers in place of NetAssist).

## Update 2026-11-10

- Own cache program `cache_probe` (`E:\loongson1B_example\cache_probe`) measured on the board: the uncached store works, a cached load returns the stale value until `cache 0x11` invalidates the line (`REPORT.md` 4.6.3). Hypothesis "store lost" rejected. Two details remain unexplained (origin of the pre-existing stale line in the instructor's run; why the debugger view of `0xA0030000` stayed old).
- Item 2 of the list below (cache line-47 check) is therefore done and superseded by `cache_probe`.

## Waiting for the board (no board available at the moment, 2026-11-09)

To do when the board and a working serial adapter are back (the CH340 adapter could not be opened on 2026-11-09, see `REPORT.md` 1.3.1):
1. Section 7 (`E:\loongson1B_example\RThread6`, already built, 0 errors): `Debug` option "Fast Download over network" must be off, open PuTTY on the CH340 COM port (115200), `Debug -> Run`, record the console output (expect 3 haircuts, 2 customers leaving; guide Figure 7.14 prints customer 1..3, the code should print 0..2).
2. Cache experiment (`example_7_2`): breakpoint at `bsp_start.S` line 47, read `0xA0030000` before and after `sw s0` (see `REPORT.md` 4.6).
3. Section 6 (serial LED): needs the 8-LED add-on board, AiCube-ISP, UART3 cable (offline analysis in `REPORT.md` 5.2).
4. Section 8 (LwIP): needs Ethernet cable and NetAssist (analysis in `REPORT.md` 5.4).
5. Section 9 (Linux): needs the instructor's files.
Board used from 2026-11-09 is a second board (`LXLINK-0102`).

## Update 2026-11-09

- No separate "final project": the course work is the guide experiments (reported by the student from the lab).
- All sections 4-9 must be done (reported by the student from the lab). Still open: Section 5 variant (odd-number sum vs cache experiment), the meaning of "own design", Section 6 hardware.

## Latest (2026-11-08)

- Cache experiment (`example_7_2`) measured on the board; one unexplained mismatch (`a2`, memory @0xA0030000 = 0xFFFE0001, expected 0x12345678). Next planned check: add a breakpoint at `bsp_start.S` line 47 (right after `sw s0`) and read `0xA0030000` before and after the store.
- `REPORT.md` audited on 2026-11-08 (section 5.1): evidence paths and hashes consistent; unverified items listed there.
- Instructor chat (screenshots, 2026-11-07/08): `example_7_2` is only **an example**, students are expected to produce **their own different design**; the course content is the same as for Chinese computer students; no lecture, lab open at 8:30, instructor arrives about 10:00. Not stated: which sections are mandatory, what the "final project" is.
- Instructor sent `AiCube-ISP-v6.96M.zip` (8.9 MB), "a serial port debugging tool mentioned in manual 6.1" (Section 6). **Not yet downloaded or inspected.** Plan: save into `E:\LoongSetup\teacher_files\AiCube`, list contents and hash only, do not run it.
- Section 6 offline analysis done (`REPORT.md` 5.2): guide code compiles with warnings only; correct BSP field is `bIntrrupt` (double r); polled mode makes `ls1x_uart_read(...,1)` blocking (code reading, not run). Questions 16-18 added to `docs\teacher_request.md`. Not started on the board: needs the LED add-on board, the UART3 DB9 cable and AiCube-ISP.

## Environment (installed and verified)

| Component | Location | Check |
|-----------|----------|-------|
| MSYS/MinGW | `C:\msys`, `C:\mingw` (on system PATH) | `make -v` -> GNU Make 3.81 |
| LoongIDE 1.1 | `D:\LoongIDE\LoongIDE.exe` | opens, Toolchain Manager lists SDE Lite 4.9.2 |
| SDE Lite 4.9.2 (MIPS GCC) | `D:\LoongIDE\mips-2015.05` | `mips-sde-elf-gcc` 4.9.2 runs |
| FTDI (EJTAG) driver | installed with LoongIDE | "Ready to use" |
| Workspace | `E:\loongson1B_example` | |
| Installers | `E:\LoongSetup` (PuTTY 0.63 portable is still unused) | |

Note: LoongIDE was installed on D: (not C: as in the guide), so all toolchain paths use `D:\LoongIDE\...`.

## Section 4 - first C project: DONE (without board)

- Project: `E:\loongson1B_example\example_4_1` (LS1B200, SDE Lite 4.9.2, bare, -O1).
- `main.c` = guide Listing 4-1. Build OK: `Build\example_4_1.exe`.
- Disassembly checked: `main` at `0x802000F4`, matches guide Figure 4.28.
- Board debugging (Section 4.2.7): DONE on 2026-11-03. Watch window: `a=-100`, `b=-10`, `x=-110`, `y=-90`; breakpoints on `main`, line 5 and line 6 hit; disassembly starts at `0x802000F4`.
- Board setup used: USB-C cable to the board `DEBUG` connector (FTDI `LXLINK`, EJTAG), 5 V adapter, optional CH340 UART cable (COM3, 115200) for the serial console.

### Debug notes (what we learned)
- Debug Options (Debug -> Options): the project had "Fast Download Application to Board (Over Network of PMON)" enabled (`DownloadByNet=1`, PMON IP 192.168.1.2). Without an Ethernet cable the first attempt hung on "starting debug..." and ended with "EJTAG server stop the user debug". We switched it off (`DownloadByNet=0`) so the program is downloaded over EJTAG (slower, a few minutes).
- One attempt printed `wait for pracc of LS232(1B/1C) fail` although the EJTAG link was fine (`prid=00004220`, LS232, 6 HW breakpoints). The board boots Linux (Buildroot) from flash automatically; a later attempt worked. The exact cause of the failure was not isolated - do not claim it was Linux without testing.
- The board runs Linux (Buildroot, console on UART5 at 115200) after every reset/power-up. "Do reset after debug stop" is enabled, so the board returns to Linux after Stop.
- Do NOT run `mtd_erase`, `devcp`, `set`/`unset` or "Program NOR Flash" without the instructor's approval - they change the board's flash/environment.
- `example_5_asm.lxp` was copied before the fix; `DownloadByNet` was set to 0 there as well.

## Section 5 - assembly: DONE ON THE BOARD (2026-11-03)

Measured on the board at the breakpoint after the store: `t0=0xfffe0001`, `t1=0x0001ffff`, `t2=0xffffffff`, `t3=0x0001fffd`, `t4=0x0001fffe`, `t5=0x80030000`; memory `0x80030000` = bytes `01 00 FE FF` (`0xFFFE0001`). Matches the emulator and the guide's Table 5-1. Details and screenshots: `REPORT.md` section 3.4, `evidence\screenshots\`. (First memory read failed with `bulk write failed: 0: 4`; the retry after a board reset worked.)

Earlier offline work (kept for reference):

- Project: `E:\loongson1B_example\example_5_asm` (copy of the BSP; `core/bsp_start.c` replaced by `core/bsp_start.S`; `main.c` removed).
- All 83 sources compile and link with the project flags (checked from the command line).
- Machine code of `bsp_start` was executed in an independent MIPS32 emulator (`emu.py`, runs the real bytes from the linked image):
  - `t0 = 0xFFFE0001` (4294836225 = 65535^2), `t1 = 0x0001FFFF`, `t2 = 0xFFFFFFFF`,
    `t3 = 0x0001FFFD`, `t4 = 0x0001FFFE`, `t5 = 0x80030000`,
    `mem[0x80030000] = 0xFFFE0001` - identical to guide Table 5-1.
- To do in the IDE: open `example_5_asm.lxp`, Project -> Compile & Build, expect 0 errors.
- To do on the board: debug, watch `t0`-`t5`, view memory at `0x80030000`.

## Remaining

| Section | Needs |
|---------|-------|
| 4.2.7 debug, 5 on board | board, USB cable, 5 V |
| 6 serial LED | board, Alapp-ISP (or PuTTY), UART3 wiring |
| 7 RT-Thread barber | board, USB-serial cable (UART5), PuTTY |
| 8 LwIP TCP/UDP | board, Ethernet cable, NetAssist, PuTTY |
| 9 Linux port | VMware, Ubuntu 16.04.7, Tftpd32, instructor's files |

## Instructor's `example_7_2.zip` (received 2026-11-03) - RUN ON THE BOARD 2026-11-03

Measured: a1=0x02C02021 (cached store invisible to uncached load), v1=0xA5A55A5A (after `cache 0x15`), but a2=0xFFFE0001 and memory @0xA0030000=0xFFFE0001 instead of the predicted 0x12345678 (the Section 5 result is still there). Cause not determined; see `REPORT.md` 4.6 and run sheet 7.2. The analysis below is from before the run.


- It is a **cache coherency experiment**, not the Section 5 odd-number sum (details in `REPORT.md`, section 4).
- Working copy: `E:\loongson1B_example\example_7_2` (paths `C:` -> `D:`, `DownloadByNet=0`, `SetBPAtFunc=bsp_start`). Original untouched at `E:\LoongSetup\teacher_files\example_7_2`.
- Command-line rebuild is byte-identical to the instructor's prebuilt `.bin` (SHA-256 `b3d8870e...81ed`).
- Known issue in the instructor's code: `s0` is overwritten, so running past the end jumps to `0x12345678`.
- Open question for the instructor: which assignment is required for Section 5?

## Documents

- `REPORT.md` - full lab report with evidence references and board run sheets (7.1, 7.2).
- `evidence\` - hashes, disassemblies, project files, emulator output.
- `docs\teacher_request.md` - missing materials and guide discrepancies.
- `docs\konspekt_MIPS32.md` - study notes.
