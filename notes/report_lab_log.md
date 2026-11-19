# Lab Report - Computer System Comprehensive Design (Loongson 1B)

Prepared: 2026-11-03. Covers work done on 2026-11-02 and 2026-11-03.
Status labels used throughout:

- **MEASURED** - observed on the real board (value read from the debugger or console).
- **VERIFIED (offline)** - checked on the PC (compiler output, disassembly, emulator, hash comparison).
- **PREDICTED** - derived by reasoning from documents; not yet measured.
- **PENDING** - still to be done or recorded.

Evidence files referenced below are in `evidence\` (hashes, disassemblies, project files, logs).

---

## 1. Environment

### 1.1 Hardware
| Item | Observation |
|------|-------------|
| Board | `1B-Board V2.0`, silkscreen date 2022.1.30 (photo) |
| Core module | `1B-Core V2.2`, silkscreen date 2021.05.26, Loongson 1B SoC (photo) |
| Debug link | USB-C cable to the `DEBUG` connector. Windows shows `USB Composite Device` `VID_0403 PID_6010`, serial `LXLINK-0103`, plus `USB Serial Converter A` and `B` (all status OK) |
| EJTAG probe seen by LoongIDE | `ejtag prid=00004220`, `mips is LS232(1B/1C)`, 6 HW breakpoints, 2 HW watchpoints |
| UART cable | `USB-SERIAL CH340 (COM3)`, `VID_1A86 PID_7523`; PuTTY 115200 shows the board console |
| Board change (2026-11-09) | From the Section 7 work on 2026-11-09 a **different physical board of the same type** is used (told by the student). The EJTAG probe reports `LXLINK-0102` (FTDI `VID_0403 PID_6010`, shown as `USB Serial Port (COM4)` and `(COM5)`) instead of `LXLINK-0103`. Results of Sections 4, 5 and the cache experiment were measured on the first board (`LXLINK-0103`); Section 7 onward on the second. Board photos in `evidence\screenshots` belong to the first board |
| Power | external 5 V adapter on the DC jack |

![Figure 1.1 - The first board (photo taken on 2026-11-02): core module on the carrier board, two DB9 connectors (the upper one is labelled UART3 on the silkscreen), RJ45, USB-C DEBUG connector at the bottom. The second board used from 2026-11-09 is of the same type.](evidence/screenshots/board_photo_1_front.jpg)

*Figure 1.1 - The first board (photo taken on 2026-11-02): core module on the carrier board, two DB9 connectors (the upper one is labelled UART3 on the silkscreen), RJ45, USB-C DEBUG connector at the bottom. The second board used from 2026-11-09 is of the same type.*

### 1.2 Software (installed and verified)
See `evidence\env\environment.txt`.

| Component | Version / location | Check |
|-----------|--------------------|-------|
| Windows | 10.0.26300.9818 | |
| MSYS/MinGW | `C:\msys`, `C:\mingw` (both added to system PATH manually) | `make -v` = GNU Make 3.81, `built for i686-pc-msys` |
| LoongIDE | 1.1, `D:\LoongIDE\LoongIDE.exe` | opens; Toolchain Manager lists the toolchain |
| Toolchain | SDE Lite 4.9.2 = `mips-sde-elf-gcc` 4.9.2 (Sourcery CodeBench Lite 2015.05-19), `D:\LoongIDE\mips-2015.05` | compiler runs |
| GDB | 7.9 | |
| FTDI driver | CDM 21228, installed from the LoongIDE installer | "Ready to use" |
| Workspace | `E:\loongson1B_example` | |
| Setup files | `E:\LoongSetup` | |

Deviations from the guide (all harmless, all handled):
1. The MSYS installer did **not** add `C:\mingw\bin` and `C:\msys\1.0\bin` to PATH; this was done manually (guide step 4.1.1 (15) anticipates it).
2. The LoongIDE installer proposed `D:\LoongIDE` instead of `C:\LoongIDE`; it was kept. Toolchain base path is therefore `D:\LoongIDE\mips-2015.05` (the guide uses `C:`).
3. Zip contains LoongIDE `1.1`; the guide names `1.1 beta2`.
4. The installers are Chinese (GBK) and display garbled text on this Windows; button positions are standard.

### 1.3 Instructor's UART driver file (inspected, not run)
`CH341SER.EXE` (received 2026-11-03 via the chat tool; 799,096 bytes; SHA-256 `458C37BDAFBE4CE3CD0BAF728C232B4B765B36D7463956E9B94CDF099212CAD1`). Authenticode signature status `Valid`, signer "Nanjing Qinheng Microelectronics Co., Ltd." (the CH340/CH341 chip vendor), countersigned chain DigiCert, certificate valid until 2027-03-04. The file was only inspected; it was **not executed**. The CH340 cable already appeared as `USB-SERIAL CH340 (COM3)` and the PuTTY console worked, so the driver was not needed on this PC. Signature validity shows the file is unmodified vendor software, not that it is safe for any other purpose.

### 1.3.1 Serial console problem (2026-11-09) - UNRESOLVED at the time of writing
Symptom: the `USB-SERIAL CH340` adapter (the cable that worked on 2026-11-03 as `COM3`, `VID_1A86 PID_7523`) is listed as working (`Status OK`, `CM_PROB_NONE`, service `CH341SER_A64` running, driver `wch.cn 4.0.2026.2`, installed 2026-11-03) but opening the port fails. PuTTY: `Unable to open connection to COM6 / Unable to configure serial port`. PowerShell `SerialPort.Open()`: first `Access to the port is denied` (PuTTY was still running), then, after PuTTY was closed, `A device attached to the system is not functioning` at 9600, 19200, 57600 and 115200.
Tried without success: other USB ports (hub ports 1, 3 and 4, the last one being the 2026-11-03 port, giving COM6, COM7, COM3), unplugging the DB9 end from the board, disable/enable of the device, a full restart (confirmed by boot time 2026-11-09 11:24), uninstall and re-plug of the device (as reported by the student). No Windows updates were installed since 2026-11-02. The FTDI probe channel `COM5` opens but showed no data in 4 s and after one Enter, so it is not used as a console. Cause not determined. Unverified ideas: adapter/driver incompatibility, other host-side state. Not tried: another serial adapter, the instructor's signed `CH341SER.EXE` (section 1.3).

Further attempts on 2026-11-10 (all unsuccessful): no separate CH340 program is installed (Installed apps has no CH340/WCH entry; the driver consists of the package `oem131.inf`, `CH341S64.SYS`, an older `CH341SER.SYS` (2023), four `CH341*.DLL` files and the service `CH341SER_A64`). The driver package was removed with "remove the driver software" (confirmed: no CH34x package in `pnputil /enum-drivers`, ghost devices COM3/COM7 not present), then reinstalled with the instructor's `CH341SER.EXE` (message "The drive is successfully Pre-installed in advance!"; package `oem131.inf` 4.0.2026.2 present again, driver store folder created 2026-11-10 10:28; the old `.SYS`/`.DLL` files stayed in System32 because no restart was done). With the cable plugged in again (`COM3`, hub port 3), opening the port at 115200 and 9600 fails with the same message. A clean driver re-install therefore did not help. The device Properties were also checked (status "working properly"; tabs General, Port Settings, Driver, Details, Events, no Power Management tab; Port Settings 9600/8/None/1/None; Advanced: FIFO buffers on, receive 14, transmit 16, `COM3` - all default values), and after the student unticked "Use FIFO buffers" the port still failed to open (`COM6` after re-plugging, same message at 115200 and 9600). The student reports that two different USB-serial cables were tried and both fail the same way on this PC, which points to the host (Windows / driver / USB side) or to a chip-driver incompatibility common to both cables (both are probably the same model), rather than to one defective cable. For comparison, the FTDI virtual port `COM5` of the board `LXLINK-0102` opened without error on 2026-11-09, so the Windows serial stack itself works; only the CH341 path fails.
Correction (2026-11-10, after inspecting the files): `CH341SER.SYS` is the 32-bit build and `CH341S64.SYS` the 64-bit build of the same WCH driver (`oem131.inf` lists both); the service `CH341SER_A64` loads only `\SystemRoot\System32\Drivers\CH341S64.SYS` (Start=3, Type=1). The older `CH341SER.SYS` (3.8, 2023) seen earlier was not in use and was overwritten by the clean re-install: both files are now version 4.00, Microsoft-signed ("Windows Hardware Compatibility Publisher"), SHA-256 `83078D87…D5CE` (`CH341S64.SYS`, 74,384 bytes) and `BD849760…10CF` (`CH341SER.SYS`, 53,904 bytes). So there is no driver-version conflict on this PC. Online research (community posts, not verified by WCH or Microsoft) points to driver-version or counterfeit-chip issues and to device-state problems; WCH's official page lists only version 4.0. Remaining untested: the cable on another PC, another serial adapter, an older driver. The LoongIDE folder also contains `FAQ-Windows...pdf` (5 pages, Chinese, 2021); it only covers the LxLink (FTDI) driver and debug-connection issues, not CH340.

### 1.3.2 Unexpected reboot (2026-11-09 11:24)
The restart done at 11:24 was a **bugcheck**: Windows event 1001/41, `0x0000001E` (KMODE_EXCEPTION_NOT_HANDLED), parameters `0xC0000005, 0x00007FF9769A1000, 0x8, 0x00007FF9769A1000`; minidump `C:\WINDOWS\Minidump\110926-18468-01.dmp` (not analysed, no debugger installed). Cause unknown; whether it is related to the CH340 problem is **not known**. After the reboot all project files, reports and project folders were intact (checked by listing, sizes and the SHA-256 of `RThread6\main.c`), and zip backups were made: `E:\LoongSetup\backup\loongson1B_example_2026-11-09.zip` (SHA-256 `42E74EA2…059B`) and `project_docs_2026-11-09.zip` (SHA-256 `BC0ED28A…A3B5`, everything in this folder except `materials`).

### 1.4 Backups
`E:\LoongSetup\backup\loongson1B_example_2026-11-02.zip` (workspace before the 2026-11-03 work) and `E:\LoongSetup\backup\main.c.orig` (original `example_4_1\main.c` before it was overwritten with the guide listing).

---

## 2. Section 4 - first C project (`example_4_1`)

### 2.1 Build - VERIFIED (offline)
- Project `E:\loongson1B_example\example_4_1`: C Executable, `LS1B200 (LS232)`, SDE Lite 4.9.2, bare programming, `-O1`.
- IDE console: `Errors: 0`, `Warnings: 0`, `Output Size: 264.23 KB`.
- MIPS & BSP options: `LS1B200 (LS232)`, `mips32`, `little-endian`, FP hardware `(none)`, FP ABI `soft-float` (as in the guide).
- Source: `evidence\example_4_1\main.c` (guide Listing 4-1).
- Disassembly of `main` (`evidence\example_4_1\main.disasm.txt`): entry `0x802000F4`; `li v0,-100` / `sb`, `li v0,-10` / `sb`, `lbu`, `addu`, `sll 0x18` / `sra 0x18` (8-bit sign extension), `sb`, `subu`, `sll/sra`, `sb`. Matches guide Figure 4.28.
- `size`: text 52520, data 3600, bss 17848. SHA-256 of `.exe`/`.bin`: `evidence\example_4_1\sha256.txt`.

### 2.2 Debug on the board - MEASURED (2026-11-03)
Watch window while stopped at the end of `main`:

| Variable | Value | Expected |
|----------|-------|----------|
| `a` | -100 | -100 |
| `b` | -10 | -10 |
| `x` (= a + b) | -110 | -110 |
| `y` (= a - b) | -90 | -90 |

![Figure 2.1 - Section 4 on the board: Watch window at the stop in `start.S` after `main` returned: a == -100, b == -10, x == -110, y == -90.](evidence/screenshots/example_4_1_watch_a_b_x_y.png)

*Figure 2.1 - Section 4 on the board: Watch window at the stop in `start.S` after `main` returned: a == -100, b == -10, x == -110, y == -90.*

Also observed: breakpoints on `main` (line 1), line 5 and line 6 were set and the program stopped at `main`; the disassembly view started at `802000F4`; CPU registers were populated; execution later reached `start.S` line 158 (`jal _sys_exit`) after `main` returned. The COM3 console printed `Clock_mask: 8000, step=100000` from the BSP timer initialisation (same line as in guide Figure 7.78).

### 2.3 Problems met during board debugging (chronological)
| # | Symptom | Finding | Resolution |
|---|---------|---------|-----------|
| 1 | Board not detected by Windows | Cable not yet connected correctly | Reconnected USB-C to the `DEBUG` connector; `LXLINK-0103` appeared |
| 2 | `Debug -> Run` stayed on "Project ... is starting debug..." for more than 4 minutes; then dialog `EJTAG server stop the user debug.` (IDE string 112001) | `mips-sde-elf-gdb` had connected to the IDE's built-in GDB server (127.0.0.1 -> 127.0.0.2:50000), so the stall was between the IDE and the board. Project had `DownloadByNet=1` and PMON IP `192.168.1.2` (fast download over Ethernet via PMON) but no Ethernet cable was connected | In `Debug -> Options -> Main` the option "Fast Download Application to Board (Over Network of PMON)" was unchecked (`DownloadByNet=0`); the IDE manual states that without network LxLink downloads over EJTAG (slower) |
| 3 | One attempt: console `wait for pracc of LS232(1B/1C) fail`, `debugger has terminated connection` | EJTAG link itself was fine (`prid=00004220`). The CPU did not answer the processor-access request. The board boots Linux (Buildroot) from flash automatically after reset | A later attempt succeeded. **The exact cause was not isolated**; the hypothesis that Linux idle (`WAIT`) blocks the debugger is untested |

IDE manual (decompiled `loongide_manual.chm`, Chinese) facts used: "Board is Running Bootloader PMON" - if unchecked, LoongIDE initialises PLL and RAM before debugging; "Fast Download ... Over Network of PMON" - downloads through PMON's TCP/IP, otherwise LxLink downloads through EJTAG (slower); "Do reset before debug start/after debug stop" options exist (project has `ResetAtStart=0`, `ResetAtStop=1`).

### 2.4 Board console observation (PuTTY, COM3, 115200)
The board runs Linux (Buildroot) from flash: `Welcome to Buildroot`, `buildroot login:`, getty on `ttyS5 115200`. Kernel/boot messages that were visible and are recorded as-is (not investigated): `EXT2-fs (mtdblock1): error: remounting filesystem read-only`, `proftpd` and `pppd` start-up errors (`/dev/ttyUSB2` option, missing `/var/run`). The RTC date is not set (`Jun 10 05:40`). No commands were typed in Linux and nothing was written to flash.

---

## 3. Section 5 - assembly (`example_5_asm`)

### 3.1 Task (guide 5.1)
Compute `1 + 3 + 5 + 7 + ... = n^2` using assembly, stopping before the unsigned 32-bit range overflows, and store the sum at `0x80030000`.

### 3.2 Implementation
Project `E:\loongson1B_example\example_5_asm` (copy of the BSP; `core/bsp_start.c` replaced by `core/bsp_start.S`; `main.c` removed). Source: `evidence\example_5_asm\bsp_start.S`. Algorithm = guide Section 5 (remaining-space pre-check, no overflow exception):

```
t0=0; t1=1; t2=-1 (0xFFFFFFFF); t3=0
loop: t4 = t2 - t0                 ; remaining space
      if (t4 < t1 unsigned) goto save
      t0 += t1; t3 = t1; t1 += 2; goto loop
save: t5 = 0x80030000; *(t5) = t0
```
`.set reorder` is used for the user code so the assembler fills delay slots; `bltu` expands to `sltu at,t4,t1` + `bnez at` + `nop` (see `bsp_start.disasm.txt`, identical to guide Figure 5 disassembly).

### 3.3 Offline verification - VERIFIED (offline)
- Command-line build of all 83 sources: 0 errors; IDE build of the project: `Errors: 0`, 22 warnings, all in vendor files (`sscanf.c` 11, `ns16550.c` 4, `strtod.c` 2, `exception.c`, `aligned_malloc.c`, `strcat.c`, `strcpy.c` 1 each); none in `bsp_start.S`.
- The real machine code from the IDE-built `.exe` was executed in an independent MIPS32 emulator (`evidence\example_5_asm\emu.py`; it decodes the raw bytes, not objdump text). Output (`emulator_run.txt`):

| Register | Value | Meaning |
|----------|-------|---------|
| `t0` | `0xFFFE0001` (4294836225) | sum = 65535^2 |
| `t1` | `0x0001FFFF` (131071) | next odd number (not added; would overflow) |
| `t2` | `0xFFFFFFFF` | max unsigned |
| `t3` | `0x0001FFFD` (131069) | last odd number added |
| `t4` | `0x0001FFFE` | remaining space |
| `t5` | `0x80030000` | store address |
| `mem[0x80030000]` | `0xFFFE0001` | stored result |

These equal the guide's Table 5-1. Independent arithmetic check: n = 65535 is the largest n with n^2 <= 2^32 - 1; last odd number = 2n - 1 = 131069; next = 131071; remaining = 0xFFFFFFFF - 0xFFFE0001 = 0x1FFFE. Passed.

### 3.3.0 Project setting corrected before the board run
`example_5_asm.lxp` had `SetBPAtFunc=main` (copied from `example_4_1`), but the project has no `main` function. It was changed to `SetBPAtFunc=bsp_start` on 2026-11-03 before the board run (`DownloadByNet=0` was already set).

### 3.3.1 Hash of the IDE-built binary
`evidence\example_5_asm\sha256.txt`; size text 52112, data 3600, bss 17848.

### 3.4 Board measurement (2026-11-03)

Procedure: opened `example_5_asm.lxp`, breakpoint on `bsp_start.S` line 47 (`move ra,s0`, the instruction after the result store), `Debug -> Run`, continued to the breakpoint. Before the run LoongIDE asked "Source file is more recent than executable. Recompile?" because `bsp_start.S` had been re-saved (new timestamp 14:06); its content was verified identical (byte compare) to the evidence copy; "Yes" was chosen. The rebuilt `.exe`/`.bin` have the same SHA-256 as the earlier verified build (`b1522481...01d3` / `9b5ada69...f1b9`), so the binary measured on the board is the binary verified in the emulator.

**Registers read at the breakpoint - MEASURED** (`evidence\screenshots\example_5_asm_debug_stopped_line47_registers.png`):

| Register | Measured | Expected | Result |
|----------|----------|----------|--------|
| t0 | `0xfffe0001` | `0xFFFE0001` | match |
| t1 | `0x0001ffff` | `0x0001FFFF` | match |
| t2 | `0xffffffff` | `0xFFFFFFFF` | match |
| t3 | `0x0001fffd` | `0x0001FFFD` | match |
| t4 | `0x0001fffe` | `0x0001FFFE` | match |
| t5 | `0x80030000` | `0x80030000` | match |

![Figure 3.1 - Section 5 on the board: stop at `bsp_start.S` line 47 (after the result store); CPU Registers show t0 = 0xfffe0001, t1 = 0x0001ffff, t2 = 0xffffffff, t3 = 0x0001fffd, t4 = 0x0001fffe, t5 = 0x80030000. The console text `wait for pracc ... fail` at the bottom is the non-fatal message discussed in 3.4.](evidence/screenshots/example_5_asm_debug_stopped_line47_registers.png)

*Figure 3.1 - Section 5 on the board: stop at `bsp_start.S` line 47 (after the result store); CPU Registers show t0 = 0xfffe0001, t1 = 0x0001ffff, t2 = 0xffffffff, t3 = 0x0001fffd, t4 = 0x0001fffe, t5 = 0x80030000. The console text `wait for pracc ... fail` at the bottom is the non-fatal message discussed in 3.4.*

Other registers seen in the same screenshot (not part of the expected set): zero `0x0`, at `0x1`, v0 `0x0`, v1 `0x80211f58`, a0 `0x0`, a1 `0x80001fe0`, a2 `0x2000`, a3 `0x20`, t6 `0x04000000` (= `_RamSize`, 64 MB), t7 `0x2000`, s0 `0x802000b8` (return address into `start.S`), s1 `0x80a498b0`, s2 `0x80a4a9dc`, s3 `0x809c0000`, s4-s7 `0x0`.

**Memory at `0x80030000` - first attempt FAILED (not a measurement).** `View Memory` (address `0x80030000`, 16 items, type "unknown") showed only `0x00` bytes, while the IDE console printed `bulk write failed: 0: 4` (repeated 8 times), `after reset LS232(1B/1C), wait for pracc fail also`, `LS232(1B/1C) read a word @0x8003000c fail`, then `debugger has terminated connection` (`evidence\screenshots\example_5_asm_view_memory_read_failed.png`, `..._console_bulk_write_failed_after_stop.png`). The all-zero display must be treated as "read failed", not as memory content. Windows still listed the FTDI/LXLINK devices as OK and logged no USB events. The `bulk write failed: 0: 4` text is interpreted (not confirmed) as an FTDI D2XX `FT_IO_ERROR` (status code 4) on the USB link. Memory read is retried (see run sheet 7.1 step 5).

**Memory at `0x80030000` - second attempt SUCCEEDED - MEASURED** (`evidence\screenshots\example_5_asm_view_memory_0x80030000_ok.png`). Procedure: board reset button, debug run again, stop at line 47, `View Memory`, address `0x80030000`, 16 items. The IDE console showed no error this time. Bytes read (in address order):

| Address | Bytes | 32-bit little-endian word | Note |
|---------|-------|---------------------------|------|
| `0x80030000` | `01 00 FE FF` | `0xFFFE0001` (= 4294836225 = 65535^2) | the stored result - matches the expected value |
| `0x80030004` | `00 00 00 00` | `0x00000000` | not written by this program |
| `0x80030008` | `86 87 00 0C` | `0x0C008786` | not written by this program (leftover RAM content) |
| `0x8003000C` | `00 00 00 00` | `0x00000000` | not written by this program |

![Figure 3.2 - Section 5 on the board: View Memory at 0x80030000, 16 bytes: 01 00 fe ff = 0xFFFE0001 (little-endian), the stored result.](evidence/screenshots/example_5_asm_view_memory_0x80030000_ok.png)

*Figure 3.2 - Section 5 on the board: View Memory at 0x80030000, 16 bytes: 01 00 fe ff = 0xFFFE0001 (little-endian), the stored result.*

Only the word at `0x80030000` is written by the program (a single `sw`); the other bytes are residual RAM content and carry no meaning for this experiment. The Disassembly pane showed the stop at `80200134: move ra,s0`, after `80200130: sw t0,0(t5)`.

**Conclusion for Section 5: the program computes the sum of the first 65535 odd numbers = 4294836225 = `0xFFFE0001` on the real Loongson 1B and stores it at `0x80030000`; all measured registers and the memory word match the emulator result and the guide's Table 5-1.**

Also observed in the first run: the console again showed `wait for pracc of LS232(1B/1C) fail` at the start, yet the session reached the breakpoint and the registers were read correctly, so that message is **not** always fatal.

---

## 4. Instructor's project `example_7_2.zip` - analysis

Received 2026-11-03, SHA-256 of the zip as received: `5f4e554f4ab2eb69180b96fa74d22a236a814314713f7b5e323aa75280bc5739` (43,930 bytes). Integrity test: OK, 44 entries, all paths inside `example_7_2/`. Nothing was executed. The contained `.exe` is a **MIPS32 ELF**, not a Windows program. Extracted copy kept unchanged at `E:\LoongSetup\teacher_files\example_7_2`.

### 4.1 Structure
Assembly-only minimal project: `start.S`, `bsp_start.S`, `cache.S`, `irq_s.S`, `ls1b_exception_handler.S`, `ls1b_interrupt_handler.S`, headers (`ls1b.h` with 300 `#define`s: UART0-5, GPIO, GMAC, CAN, I2C, SPI, PWM, RTC, NAND, interrupt controller bases), `ld.script` (`.text` at `0x80200000`), link flags `-nostartfiles -nodefaultlibs -nostdlib`. There is no `main`, no `printk`, no driver library. The exception and interrupt handlers are empty templates.

`start.S` flow: clear Status/Cause/Compare, set `gp`, clear `.bss`, set stack, `set_memory_size`, `config_cache`, `get_memory_conf`, `flush_cache` (invalidate all caches), set `Config.K0` = cached, `init_common_exception_vector`, call `bsp_start`, then `_sys_exit` (`break 1023`).

### 4.2 It is not the Section 5 task
The guide (Section 5) says to open `bsp_start.S` of the provided `example_7_2` and insert the odd-number sum code. The `bsp_start.S` in the received zip already contains **a different experiment - a cache coherency experiment**:

| Line | Instruction(s) | Address (virtual / physical) | Purpose |
|------|----------------|------------------------------|---------|
| 26-28 | `li a0,0xA5A55A5A`; `sw a0,0(0x80020000)` | kseg0 cached / `0x00020000` | write through the cache |
| 32-33 | `lw a1,0(0xA0020000)` | kseg1 uncached / `0x00020000` | read the same memory uncached |
| 36-37 | `cache 0b10101,0(0x80020000)` | | `0x15` = D-cache, **Hit Writeback Invalidate** (verified against MIPS Vol. II Tables 3-28/3-29: bits[17:16]=01 D-cache, bits[20:18]=101) |
| 40-41 | `lw v1,0(0xA0020000)` | uncached | read memory again |
| 43-45 | `li s0,0x12345678`; `sw s0,0(0xA0030000)` | uncached / `0x00030000` | uncached write |
| 47-48 | `lw a2,0(0x80030000)` | cached / `0x00030000` | cached read of the same location |

### 4.3 Reproducibility - VERIFIED (offline)
A command-line rebuild of the working copy (script `tools\build_asm_project.sh`, same flags as the IDE makefiles) produced `example_7_2.bin` with SHA-256 `b3d8870e10e53481f68f1015de23806e567d4b45d7034278068b07fdb78e81ed` - **byte-identical to the instructor's prebuilt `Build\example_7_2.bin`** (2856 bytes). `size`: text 2828, data 28, bss 16400 (identical). (A first attempt with a different link order produced the same 701 instructions at shifted addresses; the IDE links `bsp_start.o` first.) The `.exe` files differ in hash only because of embedded debug paths (`evidence\example_7_2\*`).

### 4.4 Findings in the instructor's code
1. **`s0` is overwritten.** `move s0,ra` saves the return address at the start, but `li s0,0x12345678` reuses `s0`; the final `move ra,s0` / `jr ra` therefore jumps to `0x12345678` if the program is run to the end. That is a kuseg (TLB-mapped user) address; the exact outcome (TLB exception, empty handler stub) was not tested and must not be assumed. Harmless while stopped at breakpoints; do not "Continue" past the last instruction. The code was left **unchanged** in the working copy.
2. The `.lxp` pointed to `C:\LoongIDE`; the working copy uses `D:\LoongIDE`. `DownloadByNet=1` was set to `0` (see 2.3). `SetBPAtFunc=main` was changed to `bsp_start` (there is no `main`). Original and working-copy project files are in `evidence\example_7_2\`.
3. Stored breakpoints in the instructor's original: `bsp_start.S:26`, `bsp_start.S:34`, `core\cache.S:251`, `core\cache.S:255`. For the board run the working copy was changed to `bsp_start.S:34, :43, :50, :26` (the `cache.S` entries removed; `evidence\example_7_2\example_7_2.working_copy.lxp`). The two `cache.S` lines are inside `_init_cache`, which `start.S` never calls (it calls `flush_cache`), so they will not be hit. `start.S` itself carries the comment that cache-operation assembly cannot be single-stepped.
4. The written test addresses (`0x00020000`, `0x00030000`) are in RAM below the program (`0x80200000`) and above the exception vectors.

### 4.5 Predicted result of the cache experiment - PREDICTED (to be measured)
Assumption: the D-cache is write-back and the line is allocated on the store; the whole cache was invalidated by `flush_cache` at start-up. For the board in the guide's PMON log: instruction and data cache 8 KB, 32-byte lines (that log is from the guide, not from our board).

| Register | Predicted value | Reason |
|----------|-----------------|--------|
| `a0` | `0xA5A55A5A` | loaded constant |
| `a1` | **not** `0xA5A55A5A` - the previous content of physical `0x20000` (unknown) | the store sits in the write-back cache; the uncached read goes to memory |
| `v1` | `0xA5A55A5A` | `cache 0x15` wrote the dirty line back |
| `a2` | `0x12345678` | the line at `0x30000` was not cached, so the cached load misses and reads memory (a stale value would require an earlier cached read of that line) |

If `a1` already equals `0xA5A55A5A`, the cache is not write-back for this access (or the store did not allocate); that would itself be a finding.

### 4.6 Board measurement - MEASURED (2026-11-03)
Build: IDE reported 0 errors, 0 warnings ("Nothing to be done", the `Build` directory was already current); `Build\example_7_2.bin` SHA-256 = `b3d8870e10e53481f68f1015de23806e567d4b45d7034278068b07fdb78e81ed` (identical to the instructor's prebuilt `.bin`); `.exe` SHA-256 = `4b3be523e809d225c5ee3cb3cf12350c1a9e5a93a9a15a9038c8a9e83bb60653`. Debug over EJTAG (`DownloadByNet=0`), no errors during the run. Screenshots: `evidence\screenshots\example_7_2_*.png`.

Stops (Continue between them, no single-stepping through `cache`):

| Stop | Address | Registers | Note |
|------|---------|-----------|------|
| line 26 (before `li a0`) | `0x802000F4` | a0=0x00000000, a1=0x80001fe0, v1=0x80204b38, a2=0x00002000 | breakpoint halts **before** the instruction executes; these are leftovers, not experiment data |
| line 34 (after `lw a1`) | `0x80200110` | a0=0xA5A55A5A, v0=0xA0020000, **a1=0x02C02021** | prediction confirmed: a1 is not 0xA5A55A5A |
| line 43 (after `lw v1`) | `0x80200128` | **v1=0xA5A55A5A** | prediction confirmed: `cache 0x15` wrote the dirty line back |
| line 50 (after `lw a2`) | `0x80200140` | v0=0x80030000, s0=0x12345678, **a2=0xFFFE0001** | **prediction wrong** (expected 0x12345678) |

View Memory (single `unsigned int`, fetched after the last stop):

| Address | Value (DEC shown) | Hex (converted) |
|---------|-------------------|-----------------|
| `0xA0020000` | 2779077210 | 0xA5A55A5A (matches v1) |
| `0xA0030000` | 4294836225 | **0xFFFE0001** (expected 0x12345678) |

![Figure 4.1 - Cache experiment, run 1, stop at line 34: a0 = 0xA5A55A5A, a1 = 0x02C02021 (uncached load did not see the cached store).](evidence/screenshots/example_7_2_stop2_line34_a1_stale.png)

*Figure 4.1 - Cache experiment, run 1, stop at line 34: a0 = 0xA5A55A5A, a1 = 0x02C02021 (uncached load did not see the cached store).*

![Figure 4.2 - Cache experiment, run 1, stop at line 50: v0 = 0x80030000, s0 = 0x12345678, a2 = 0xFFFE0001 (not the predicted 0x12345678).](evidence/screenshots/example_7_2_stop4_line50_a2_unexpected.png)

*Figure 4.2 - Cache experiment, run 1, stop at line 50: v0 = 0x80030000, s0 = 0x12345678, a2 = 0xFFFE0001 (not the predicted 0x12345678).*

![Figure 4.3 - Cache experiment, run 1: View Memory at 0xA0030000 after the store: 4294836225 = 0xFFFE0001.](evidence/screenshots/example_7_2_viewmem_0xA0030000.png)

*Figure 4.3 - Cache experiment, run 1: View Memory at 0xA0030000 after the store: 4294836225 = 0xFFFE0001.*

Note: an earlier View Memory display of `0x80030000` (bytes `01 00 fe ff`) was left over from Section 5 and was not a fresh fetch; it is not used as a measurement (screenshot `example_7_2_viewmem_stale_display_not_a_measurement.png`).

Interpretation:
- Cached store to physical `0x20000` was invisible to the uncached load (a1), and became visible after `cache 0x15` (v1). This is the intended cache-coherency effect.
- `0xFFFE0001` is exactly the result of the Section 5 odd-number sum, stored earlier at `0x80030000` (same physical word `0x30000`). Both the cached load (a2) and the memory view of `0xA0030000` returned this old value, so `0x12345678` is not visible in RAM at that address after `sw s0, 0(0xA0030000)`.
- **Cause not determined.** Not tested: whether the uncached store reached RAM at all (e.g. read `0xA0030000` right after line 45), and whether the debugger's memory read honours the kseg1 address. The prediction in 4.5 for `a2`/`0x30000` was wrong; the assumption that the line was uncached and RAM held the new value is not supported by the data. Do not claim a stale cache line from this data alone.

### 4.6.1 Second run of the cache experiment - MEASURED (2026-11-09)
Board: probe `LXLINK-0103` (a different board from the 2026-11-09 morning, identifier equal to the first board). Same binary (`.bin` unchanged, source unchanged). Breakpoints `bsp_start.S` 43, 47, 50 (`evidence\example_7_2\example_7_2.run2_breakpoints_43_47_50.lxp`). Screenshots `evidence\screenshots\example_7_2_run2_*.png`.

| Moment | Observation |
|--------|-------------|
| Stop at line 43 (`0x80200128`, `lw v1` done, `li s0` not yet) | a0 = 0xA5A55A5A, a1 = 0x02C02021 (same as run 1), v1 = 0xA5A55A5A |
| View Memory `0xA0030000` before the store | 134266867 = **0x0800BFF3** |
| Stop at line 47 (`0x80200138`, after `sw s0, 0(0xA0030000)`) | s0 = 0x12345678, v0 = 0xA0030000 |
| View Memory `0xA0030000` after the store (fetched twice) | 134266867 = **0x0800BFF3** (unchanged) |
| Control read `0xA0020000` (fresh fetch, different address) | 2779077210 = 0xA5A55A5A (shows the fetch is live) |
| Stop at line 50 (`0x80200140`, after `lw a2, 0(0x80030000)`) | **a2 = 0x0800BFF3** |

![Figure 4.4 - Cache experiment, run 2: View Memory at 0xA0030000 before the store: 134266867 = 0x0800BFF3.](evidence/screenshots/example_7_2_run2_viewmem_0xA0030000_before_store.png)

*Figure 4.4 - Cache experiment, run 2: View Memory at 0xA0030000 before the store: 134266867 = 0x0800BFF3.*

![Figure 4.5 - Cache experiment, run 2: control read at 0xA0020000 (fresh fetch): 2779077210 = 0xA5A55A5A.](evidence/screenshots/example_7_2_run2_viewmem_0xA0020000_control.png)

*Figure 4.5 - Cache experiment, run 2: control read at 0xA0020000 (fresh fetch): 2779077210 = 0xA5A55A5A.*

![Figure 4.6 - Cache experiment, run 2: View Memory at 0xA0030000 after the store (second fetch): still 134266867 = 0x0800BFF3.](evidence/screenshots/example_7_2_run2_viewmem_0xA0030000_after_store_second_fetch.png)

*Figure 4.6 - Cache experiment, run 2: View Memory at 0xA0030000 after the store (second fetch): still 134266867 = 0x0800BFF3.*

![Figure 4.7 - Cache experiment, run 2, stop at line 50: a2 = 0x0800bff3, s0 = 0x12345678.](evidence/screenshots/example_7_2_run2_stop_line50_a2.png)

*Figure 4.7 - Cache experiment, run 2, stop at line 50: a2 = 0x0800bff3, s0 = 0x12345678.*

What this shows: on this board the word at physical `0x30000` held `0x0800BFF3` before the store (on the first board it held `0xFFFE0001`, the Section 5 result, which supports the idea that it is leftover content of earlier activity on that board - not proven). After the uncached store of `0x12345678`, **both** the debugger read of `0xA0030000` and the program's cached load from `0x80030000` still return the old value. `a1`, `v1` and the `0xA0020000` readings are identical in both runs.

Two explanations fit and the data cannot separate them (neither is tested):
- H1: the uncached store did not change the memory word that either path reads (store lost, buffered, or going elsewhere).
- H2: the store did reach RAM, but a stale valid D-cache line for physical `0x30000` exists and both the cached `lw` and the debugger's read are served from it.
A discriminating program (own design, not yet written): after the uncached store, execute `cache 0x11` (Hit Invalidate, D-cache) on `0x80030000`, then load from `0x80030000`: H2 gives `0x12345678`, H1 gives the old value; a `sync` after the store and a read-back through `0xA0030000` can be added.

### 4.6.2 Own design: `cache_probe` - built offline, PREDICTED (2026-11-09)
Project `E:\loongson1B_example\cache_probe` (copy of the `example_7_2` project, new `bsp_start.S` with comments; copy of the source and project file in `evidence\cache_probe\`). It performs the instructor's uncached-store / cached-load idea in two phases and keeps all results in registers; the single breakpoint is at line 89 (`move ra, s0`, address `0x8020017C`); `s0` is never overwritten, so the program can run to its end.
Command-line build (`tools\build_asm_project.sh`, same flags as the IDE): 0 errors, 0 warnings in `bsp_start.S`; `.bin` SHA-256 `3c2fa3035de3ea6005df19f51316e82e999286f46a61f2bcc61f1395c0c87faf`, `.exe` `143f151a65901422fc67bac1e942b57a864e58c5b95bbec11737921609f25cc8` (`evidence\cache_probe\`); disassembly checked: `cache 0x11,0(a2)` = `0xBCD10000`, `sync` = `0x0000000F`. The IDE build will be compared with this.

Phase A (physical `0x30000`): t0 = uncached load, t1 = cached load (fills the line), `sw` to `0xA0030000`, `sync`, t2 = uncached load, t3 = cached load, `cache 0x11` (hit invalidate D) on `0x80030000`, t4 = cached load, t5 = uncached load. Phase B (physical `0x40000`, control, no earlier access): t6 = uncached before, `sw`, `sync`, t7 = uncached after, t8 = cached after. `a0` = `0x12345678`.

Predictions written before the run (OLD = the word's previous content):

| Register | Normal cache behaviour (store reaches RAM) | Store lost (H1) |
|----------|--------------------------------------------|-----------------|
| t0, t1 | OLD | OLD |
| t2 | `0x12345678` (RAM updated) | OLD |
| t3 | OLD (stale line filled by t1 - textbook incoherency) | OLD |
| t4 | `0x12345678` (line invalidated, refill from RAM) | OLD |
| t5 | `0x12345678` | OLD |
| t6 | OLD2 (previous content of `0x40000`) | OLD2 |
| t7, t8 | `0x12345678` | OLD2 |

The deciding registers are t2 (program-level uncached read, does not depend on the debugger) and t7. If t2 and t7 are `0x12345678`, the uncached store works and the earlier result is the pre-existing stale line (H2); if they show OLD, the store does not change memory as seen by the CPU (H1) and the cause must be investigated further.

### 4.6.3 `cache_probe` on the board - MEASURED (2026-11-10)
Board probe `LXLINK-0103`. IDE build: 0 errors, 1 warning (assumed the same vendor-file warning as the command-line build; not itemised); `Build\cache_probe.bin` SHA-256 `3c2fa3035de3ea6005df19f51316e82e999286f46a61f2bcc61f1395c0c87faf` = command-line build; `.exe` `0e758e5c2db4c8df79136f151e404c2ced83175c8b7e29b59988db17daeb2cfa` (`evidence\cache_probe\ide_build_sha256.txt`). One breakpoint at `bsp_start.S` line 89 (`0x8020017C`), reached after the entry stop and one Continue; no errors during download or run. Screenshot: `evidence\screenshots\cache_probe_stop_line89_registers.png` (values below were read from this screenshot).

| Register | Measured | Prediction (normal cache behaviour) | Result |
|----------|----------|-------------------------------------|--------|
| a0 | 0x12345678 | stored value | match |
| t0 (uncached, before) | 0x0800BFF3 | OLD | match |
| t1 (cached, before) | 0x0800BFF3 | OLD | match |
| t2 (uncached, after store+sync) | **0x12345678** | 0x12345678 | match |
| t3 (cached, after store+sync) | **0x0800BFF3** | OLD (stale line) | match |
| t4 (cached, after `cache 0x11`) | **0x12345678** | 0x12345678 | match |
| t5 (uncached, after invalidate) | 0x12345678 | 0x12345678 | match |
| t6 (phase B uncached, before) | 0x3C10800A | OLD2 | n/a (previous content) |
| t7 (phase B uncached, after) | 0x12345678 | 0x12345678 | match |
| t8 (phase B cached, after) | 0x12345678 | 0x12345678 | match |
| s0 | 0x802000B4 | unchanged return address | match |

![Figure 4.8 - Own program `cache_probe` on the board, stop at line 89: t0 = 0x0800bff3, t1 = 0x0800bff3, t2 = 0x12345678, t3 = 0x0800bff3, t4 = 0x12345678, t5 = 0x12345678, t6 = 0x3c10800a, t7 = 0x12345678, t8 = 0x12345678.](evidence/screenshots/cache_probe_stop_line89_registers.png)

*Figure 4.8 - Own program `cache_probe` on the board, stop at line 89: t0 = 0x0800bff3, t1 = 0x0800bff3, t2 = 0x12345678, t3 = 0x0800bff3, t4 = 0x12345678, t5 = 0x12345678, t6 = 0x3c10800a, t7 = 0x12345678, t8 = 0x12345678.*

Conclusions supported by this run:
- **H1 (the uncached store is lost) is rejected.** The program's own uncached load (t2, t7) sees the stored value, so the store reaches memory.
- The cached load t3 returned the old value although memory already held the new one, and returned the new value after `cache 0x11` (t4): the classic cache-incoherency effect, reproduced and explained with an explicit invalidate. In this program the stale line was created by our own earlier cached load (t1), so this part is expected by construction.
- Phase B (a word never touched before, cached load only after the store) shows no staleness (t8 = new), consistent with a normal cache.

Not explained and not tested: (a) in the instructor's program the cached `lw a2` already returned an old value without any earlier cached access in that program, so a valid D-cache line for physical `0x30000` existed before (origin unknown: earlier activity on the board such as the previous session, boot code or the debugger's own memory reads are candidates; on the first board the old value equalled the Section 5 result); (b) the debugger's `View Memory` of `0xA0030000` still showed the old value after the store, while the program's own uncached load (t2 here) saw the new one, which suggests the debugger's read path is not purely uncached; this was not isolated. Measured values were read from a screenshot; no register dump file was saved.

### 4.7 Open question for the instructor
Which assignment is required for Section 5: the odd-number sum, the cache experiment, or both? Listed in `docs\teacher_request.md`.

---

## 5. Discrepancies found in the experiment guide

| # | Place | Observation |
|---|-------|-------------|
| 1 | 9.2.7 | Text: `rootfs.ext2` in `output/image`, flashed to `/dev/mtd1`. Figure 9.49: build creates `output/images/rootfs.yaffs2` |
| 2 | Section 9 | Text: board connected directly to the PC. Screenshots: PC on Wi-Fi 192.168.0.104 (router) |
| 3 | 9.2.5 | `tar` command uses `gcc-4.3-ls232.tar.gz`; the file list says `gcc-4.3-ls232.tar` |
| 4 | 8.2.3 | In `lwip_test.h` the include-guard `#endif` is placed right after `#define _LWIP_TEST_H` |
| 5 | 8.2.14/8.2.15 | UDP tests say NetAssist protocol "TCP Server" / "TCP UDP" |
| 6 | 8.2.11 | PC address 192.168.1.111 is not listed as a step |
| 7 | 9.2.10 | host IP given as both 192.168.0.10 and 192.168.0.104 |
| 8 | 6 | "set the UART3 program to false mode" = `.bIntrrupt = false` in `ls1x-drv\uart\ns16550.c`, struct `ls1b_UART3` (Figure 6.1 shows exactly this line; the BSP field name has a double "r"; an earlier version of this report misspelled it). With `false` the UART is polled, which changes how the guide's `ls1x_uart_read(..., 1)` behaves - see 5.2 |
| 9 | 5 | Text says overflow raises an exception; the code uses `addu` (never traps) with a pre-check |
| 10 | 4.1.2 | guide names `1.1 beta2`, zip has `1.1`; MSYS installer does not set PATH |
| 11 | 7.2 / Fig 7.14 | The code prints `barber find customer %d` with `number` starting at 0 and incremented after the print, so it should print 0, 1, 2; Figure 7.14 shows 1, 2, 3 |
| 12 | 7.2 Fig 7.4 / 7.5 | Figure 7.4 shows YAFFS2, Simple GUI and LVGL ticked; Figure 7.5 (summary) says "Used components count: 0". The text does not say which to select |
| 13 | 7.2 | Step 6 says to delete the template's example code and add Listing 11-59, but the listing has no `#include` lines; the template `main.c` includes `<time.h>`, `"rtthread.h"`, `"bsp.h"`, which the listing needs (it uses `rt_*`, `ls1x_drv_init`, `install_3th_libraries`). Also: text says "3 semaphores and 1 variable" but lists 4 items (one is the mutex); listing numbers 11-58/11-59 come from another book; entry function `customer_thread(int)` is passed where `void (*)(void *)` is expected |

---

## 5.1 Audit of this report (2026-11-08)

Re-checked on 2026-11-08: every `evidence\...` path named in this report and in `PROJECT_STATUS.md` exists; the recorded SHA-256 values of `example_4_1`, `example_5_asm` and the `example_7_2` `.bin` equal the hashes of the files now on disk; the `bsp_start.S` copies in `evidence\` are byte-identical to the project files.

Not independently re-checked (taken from earlier work, screenshots or the guide):
- the 22 warnings of the `example_5_asm` build and the vendor-file breakdown;
- the 10 guide discrepancies in section 5 (from reading the guide text and figures; PDF diagrams of the MIPS manuals were not fully read);
- the cache geometry in 4.5 (8 KB, 32-byte lines) comes from a log printed in the guide, not from this board;
- the interpretation of `bulk write failed: 0: 4` and the cause of `wait for pracc ... fail`;
- all board values were read from screenshots of the IDE; no raw log files were saved.

Open technical item: the cause of `a2 = 0xFFFE0001` in the cache experiment (section 4.6).

## 5.2 Guide Section 6 (serial-controlled running LED) - offline analysis, 2026-11-08

Status of this offline analysis: **VERIFIED (offline)** by compiling and by reading the BSP source; the board run of 2026-11-10 is in 5.2.1 below. At the time of this analysis **nothing had been run on the board.** The `AiCube-ISP-v6.96M` tool (the guide's Alapp-ISP replacement) and the LED add-on board were not available at this point.

Method: the guide code (paragraphs after "6.2 Design Steps", 141 lines) was extracted unchanged from the guide `.docx` into `evidence\example_6_led_guide_main.c.txt` (blank lines of the original are not preserved). It was built in an isolated copy of `example_4_1` (`E:\LoongSetup\builds\ex6_check`; the project itself was not modified) with the project's own compiler flags, with `.bIntrrupt = false` set for UART3 in the copy as in Figure 6.1. Compile log of `main.c`: `evidence\example_6_led_main_compile_log.txt`. Link succeeded.

Findings:
1. All API names used by the guide exist in the BSP: `gpio_enable`, `gpio_write` (`core\include\ls1b_gpio.h`), `ls1x_uart_init/open/read/write`, `devUART3` (`ls1x-drv\include\ns16550.h`), `delay_ms` (`core\include\tick.h`), `ls1x_drv_init`, `install_3th_libraries` (`include\bsp.h`).
2. `main.c` compiles with warnings only: `strncmp`, `atoi`, `strcmp` are implicitly declared (`<string.h>`, `<stdlib.h>` missing), and `ls1x_uart_read(..., 1)` passes the integer `1` as a `void *` argument. The BSP converts it back with `(int)arg`, so it works, but it is a timeout value, not a pointer.
3. `BSP_USE_UART3` is already defined in `include\bsp.h`, and `ls1x_drv_init()` already calls `ls1x_uart_init(devUART3, NULL)`; the guide's `uart_init()` initialises it a second time (not shown to be harmful; not tested).
4. **Polled vs interrupt mode (by code reading, not tested on the board).** The BSP default for UART3 is `.bIntrrupt = true`. In `NS16550_read`: interrupt mode returns after at most `timeout` ms; polled mode (`false`) with a non-zero timeout calls a blocking read that loops until a byte arrives. With the guide's `false` setting and `ls1x_uart_read(devUART3, &byte, 1, 1)`, the main loop stops at the read until a byte is received, so `START` would advance the running LED by one step per received byte instead of running continuously. `ON n` / `OFF` are unaffected. The guide's own test (Figure 6.4, `ON 3`) works in either mode. The BSP source comment on the console port says interrupt mode on the console UART "affects debugging", which is probably why the guide switches UART3 to polled mode; this reasoning is a guess.
5. Latency in the running state: each loop iteration reads at most one byte and then calls `delay_ms(speed)`, so a 5-byte command such as `STOP\n` needs about 5 x `speed` ms (about 2.5 s at 500 ms) to be received completely (interrupt mode; in polled mode see item 4).
6. If the terminal sends `\r\n`, the `\r` ends the command and the following `\n` produces an empty command; the echo sends the `\0` terminator byte as part of the reply.
7. Not verifiable without hardware documents: the LED pin numbers (GPIO 9, 11, 15, 17, 20, 22, 6, 7), the active-low assumption (`gpio_write(pin, 0)` = on), possible pin-mux conflicts with UART3, and which DB9 connector is UART3. Figure 6.2 shows an 8-LED add-on board on the 40-pin header; whether such a board is available is unknown.

Figures 6.1-6.5 were extracted from the guide and viewed (6.1 = `ns16550.c` UART3 structure; 6.2 = wiring photo; the others were not examined in detail here).

### 5.2.1 Guide Section 6 on the board, 2026-11-10

Setup: LED add-on board (8 LEDs) wired to the board headers by the student; the instructor changed some pin assignments (student report), after which the student re-wired LED5-LED8 to Table 8.14 of the guide: LED1..LED8 = GPIO 9, 11, 15, 17, 20, 22, 6, 7. `main.c` was changed accordingly (`evidence\example_6_led\main.c.guide_table_20_22_6_7`; the earlier GPIO 42-45 variant `main.c.table_8_14_wiring` was **not** run). Build after `Make Clean` + `Compile & Build`: 0 errors, 4 warnings, `.exe` SHA-256 `f2bb2327a6bd361a7aa9c5de940b2a3a07f15cccf816380c2d026ef14e908d19` (`evidence\example_6_led\example_6_led_ide_build_sha256_gpio_20_22_6_7.txt`). The first IDE build did not relink (old `.exe` hash `62519e22...`, the known makefile quirk); the physical wiring was not verified independently (no multimeter); the GPIO constants in the binary were not confirmed by disassembly.

Serial path: CH340 adapter on `COM7` (115200, 8N1), cable on the DB9 labelled UART3. Terminal: AiCube-ISP 6.96M (instructor's tool), tab `CDC/HID UART Helper`; only the serial helper was used (no `Download/Program`). PuTTY showed `Unable to configure serial port` once; re-plugging the USB end of the CH340 cable fixed it (cause not proven). PuTTY flow control was `XON/XOFF` and was set to `None`. The PuTTY log of this session contains only its header line (the serial helper of AiCube was used for the test).

MEASURED (screenshots `aicube_OFF_14-49.png`, `aicube_ON6_14-51.png`, photo `photo_ON6_one_LED_lit.webp`):
- Board answer on UART3: after `OFF` + CR LF (TX 5 bytes) the receive side counted 6 bytes, shown as `OFF.` and `.` at 14:49:34.947 and 14:49:35.446 (the dot is the `\0` byte, matching the echo of the guide code, section 5.2 items 6). After `ON 6` + CR LF at 14:51:54.379 the board returned `ON 6..` at 14:51:55.113. The IDE console kept the message `wait for pracc ... fail` and the status `starting debug...` while the program was running (the same non-fatal message as in Figure 3.1); how long the download took was not measured.
- A command sent without line terminator (`ON 2`, `SendData`) is not executed (the code waits for `\r` or `\n`); two such commands are concatenated in the buffer (`ON 2ON 1..`, 14:51:32.683). This explains the first failed LED test of 14:42.
- Photo after `ON 6`: exactly one LED lit, the others dark. The silkscreen numbers are not readable in the photo, so which LED it is cannot be shown from the photo alone.
- AiCube prefixes its own log lines (sent: red, received: blue) with a garbled string. Comparison with guide Figure 6.4 (same tool, Chinese display: `发送→OFF` in red, `接收←OFF.` in blue) shows that these are the Chinese labels "sent" and "received" displayed in the wrong code page on this PC (identified on 2026-11-12 from the guide figure; the byte-level encoding was not checked). The echo format `OFF.` / `ON 3.` is the same as in the guide figure. The RX counter shows what arrived from the board. In the `OFF` screenshot RX = 6 bytes, consistent with a stray `S` + `OFF` + `\0` + one more `\0` (the stray `S` is a leftover byte in the board's buffer; its origin was not determined).

All screenshots of `evidence\screenshots`, `evidence\example_6_led` and `evidence\section9_hello` (47 files) were opened and compared with the values in this report on 2026-11-12; no value mismatch was found. One note: the first board's Linux boot log of 2026-11-03 already showed `EXT2-fs (mtdblock1): error: remounting filesystem read-only` (screenshot `putty_com3_linux_boot_then_clock_mask.png`); the same device (`mtdblock1`) is the one that failed with the kernel panic on 2026-11-10 (other board), so a pre-existing weakness of the Linux root file system on these boards is possible, not proven.

REPORTED BY THE STUDENT (observation, no screenshot or video kept): `ON 1`, `ON 2`, `ON 3` and further `ON n` light the correct LED; `START` makes the LEDs change/run; "everything is correct, all observations done". A video of `START` was supplied by the student: `evidence\example_6_led\START_video.mp4` (717,050 bytes, SHA-256 `47e33c73ee2c9d71f8a7e3c30d9e8c968c0817e754f13ae60b7e48e5bcdb0303`, source file time 14:58 on 2026-11-10, copied from the student's chat-app video folder). **The video was not watched during this work**; its content is the student's statement. `SPEED n` and `STOP` were **not** documented separately; the polled-mode behaviour (item 4 above: `START` advancing per received byte) was **not** tested or recorded.

**Polled vs interrupt mode, second run (student observations, 2026-11-10, no video of these steps):**
- With the guide's setting `.bIntrrupt = false` (build `f2bb2327...`): `START` sent with `SendCRLF` moved the LEDs only 1-2 steps and then they stopped; with AiCube `Auto Send` (a byte every 100 ms) the LEDs kept stepping. This matches the code reading of 5.2 item 4 (blocking read in polled mode, one LED step per received byte).
- With `.bIntrrupt = true` (BSP default; only line 890 of `ns16550.c` changed, `evidence\example_6_led\ns16550.c.UART3_bIntrrupt_true`; build after `Make Clean`, `.exe` SHA-256 `3218f1188407085f09413a2113c3ab9a009ce773eb539556282afb95bb9c1f5d`): `START` made the LEDs run continuously without further input.
- `SPEED 100` / `SPEED 1000` changed the speed (student: "works", the effect first looked small). `STOP` did not stop the LEDs at once; they stopped after waiting about 10 s. This agrees with 5.2 item 5: only one byte is read per loop iteration followed by `delay_ms(speed)`, so a command of n bytes needs about n x `speed` ms (at 1000 ms about 5 s for `STOP` + CR). The delay was not timed with a stopwatch.
- Screenshot of the `STOP` step: `evidence\example_6_led\aicube_STOP_15-08.png` (TX 82, RX 50). It shows the board returning `STOPSTOPSTOP.` at 15:07:55.452 (three `STOP` sent without terminator before; the buffer accumulated them, as described above) and `STOP..` at 15:08:24.494 after `STOP` was sent at 15:08:22.418 and the line terminator at 15:08:23.745. The echo is therefore seen about 0.75 s after the terminator; the LED stop itself is the student's observation and is not visible in the screenshot.
- Conclusion (observed, mechanism from code reading): the guide's polled setting prevents a continuously running LED; interrupt mode (BSP default) gives the behaviour the guide describes.

PENDING: confirmation of the silkscreen numbering of the LED module; a video of `SPEED`/`STOP` was not recorded.

![Figure 6.6 - AiCube-ISP serial helper: `ON 6` sent at 14:51:54.379 (red), board echo `ON 6..` received at 14:51:55.113 (blue); earlier lines show the concatenated `ON 2ON 1..`. TX 59, RX 60.](evidence/example_6_led/aicube_ON6_14-51.png)

![Figure 6.7 - LED module after `ON 6`: exactly one LED lit (student states it is LED6; silkscreen not legible in the photo).](evidence/example_6_led/photo_ON6_one_LED_lit.webp)

![Figure 6.8 - AiCube-ISP serial helper after `OFF` + CR LF: TX 5, RX 6 bytes; the board returns `OFF.` and `.`.](evidence/example_6_led/aicube_OFF_14-49.png)

## 5.3 Guide Section 7 (RT-Thread Sleeping Barber) - offline analysis, 2026-11-09

Status: **PREDICTED / read from sources**. Nothing built or run yet. Guide text (7.1, 7.2, 7.2.1) read in full; Figures 7.3, 7.4, 7.5 and 7.14 viewed.

Facts read from the installed LoongIDE template (`D:\LoongIDE\Template\RTThread`, RT-Thread 4.0.2 per `Template.ini`):
- `rtconfig.h`: `RT_THREAD_PRIORITY_MAX 32`, `RT_TICK_PER_SECOND 1000` (so `rt_thread_mdelay(50)` = 50 ticks), `RT_USING_SEMAPHORE`, `RT_USING_MUTEX`, `RT_USING_HEAP`, `RT_USING_CONSOLE`, `RT_USING_FINSH`, `RT_MAIN_THREAD_PRIORITY 10`, main stack 8 KB.
- `rt_thread_mdelay` exists (`include\rtthread.h`, `src\thread.c`).
- The heap runs from `__bss_end` to the end of RAM (`port\bsp_start.c`), so six threads with 20 KB stacks (120 KB) fit in 64 MB.
- Template `main.c` includes `<time.h>`, `"rtthread.h"`, `"bsp.h"` and contains a demo thread; the project console is UART5 (`BSP_USE_UART5 // Console_Port` in `bsp.h`), as the guide's UART5 cable assumes.

Behaviour expected from the code (reasoning, not run). In RT-Thread a smaller priority number means a higher priority. Barber priority 10 (same as `main`), customers 5, 6, 7, 8, 9 (customer 1 highest). Each customer sleeps 50 ms, then in priority order: customers 1, 2, 3 find a free seat (`freeseats` 3 to 2, 1, 0; prints `freeseats=2`, `=1`, `=0`) and block on `barber`; customers 4 and 5 find `freeseats == 0` and print `customers 4 leave.` / `customers 5 leave.`. The barber (lowest priority) then serves three customers: three times "find customer", "begin to cut hair", about 40 ms, "finish cut hair". Invariant that should hold on the board: exactly 3 haircuts and exactly 2 customers leaving. Exact interleaving of the start-up messages with the `msh />` prompt may differ.

Difference to the guide figure: the code should print `barber find customer 0 / 1 / 2` (discrepancy 11), Figure 7.14 shows 1 / 2 / 3. On the board we expect 0 / 1 / 2 with the guide code unchanged; this is a prediction to be checked.

Build (2026-11-09) - **VERIFIED (offline)**: project created with the New Project Wizard at `E:\loongson1B_example\RThread6` (LS1B200, SDE Lite 4.9.2, RT-Thread 4.0.2, no components, `-O0 -g -Wall`, defines `LS1B;OS_RTTHREAD`). `main.c` = template `#include` lines + guide Listing 11-59 unchanged (`evidence\example_7_barber\main.c`; template original kept as `main.c.template_original`). IDE result: `Errors: 0`, `Warnings: 11`, output 628.59 KB, 16.55 s. `Build\RThread6.exe` SHA-256 `93a8c2412ed32bb325595fe1e25f638cc8c9b3ad11814f8663f6d64b55847bda`, `.bin` `e3d6ab9c182e0c195e077d34fc048147d1a81ea133dc25e151ae164bf61387db`; size text 252940, data 5080, bss 24016 (`evidence\example_7_barber\`). Re-compiling `main.c` alone with the project flags gives 4 warnings from our file (log: `main_compile_log.txt`): unused variable `i` in `barber_thread`; `res` set but not used in `customer_thread`; `customer_thread(int)` passed as `void (*)(void *)` (incompatible pointer type); `num[i]` (int) passed as `void *`. The other 7 of the 11 warnings come from other files and were not itemised. The last two are formally type errors in the guide code; on this 32-bit MIPS ABI an `int` and a pointer are passed the same way, so the customer number (0..4) should arrive intact - to be confirmed by the console output (`customers 1..5`).

**Board run - MEASURED (2026-11-10).** Board probe `LXLINK-0103`, console on the CH340 adapter (`COM7`, PuTTY, 115200, session log `evidence\example_7_barber\putty_COM7_20261110_105614.log`, SHA-256 `8d23b94263c00b22c1095aec75e7bb1e6dc073e8980a269e2151815e90546b44`). Download over EJTAG (`DownloadByNet=0`), `Debug -> Run`; screenshots `example_7_barber_ide_debug_running.png`, `example_7_barber_putty_output.png`. Binary: the `RThread6.exe`/`.bin` of the offline build above (the `.bin` hash `e3d6ab9c...87db` was re-read unchanged before the run). The console printed (raw log, in order): `Clock_mask: 8000, step=100000`; RT-Thread banner `4.0.3 build Nov 9 2026`; `Welcome to RT-Thread.`; creation messages of the two semaphores and the mutex; `barber thread startup.`; `customers 1..5 thread startup.`; `msh />freeseats=2`, `customers 1 is waitting to cut hair.`, `freeseats=1`, `customers 2 is waitting...`, `freeseats=0`, `customers 3 is waitting...`; `customers 4 leave.`, `customers 5 leave.`; then three times `barber find customer N .` / `barber begin to cut hair.` / `barber finish cut hair.` with N = 0, 1, 2.

| Prediction (this section, before the run) | Measured |
|--------------------------------------------|----------|
| exactly 3 haircuts and exactly 2 customers leaving | 3 x "finish cut hair"; "customers 4 leave", "customers 5 leave" - match |
| `freeseats` printed 2, 1, 0 in that order | match |
| customers 1, 2, 3 wait, customers 4, 5 leave | match |
| the code prints `barber find customer` 0, 1, 2 (guide Figure 7.14 shows 1, 2, 3) | **0, 1, 2 measured** - prediction confirmed; discrepancy 11 is real |
| the integer entry parameter passed through the `void *` argument arrives intact (customer numbers) | `customers 1..5` printed correctly - the incompatible-pointer warnings are harmless on this target |

![Figure 7.1 - Section 7 on the board: PuTTY console (COM7, 115200) with the Sleeping-Barber output: freeseats = 2, 1, 0; customers 1-3 wait; customers 4 and 5 leave; the barber serves customers 0, 1, 2.](evidence/screenshots/example_7_barber_putty_output.png)

*Figure 7.1 - Section 7 on the board: PuTTY console (COM7, 115200) with the Sleeping-Barber output: freeseats = 2, 1, 0; customers 1-3 wait; customers 4 and 5 leave; the barber serves customers 0, 1, 2.*

Everything else in the output is identical to guide Figure 7.14 except the barber's customer counter (0-based here, 1-based in the figure) and the start-up banner lines above the figure's first line. The program was left running; the run was stopped from the IDE afterwards (not recorded here).

Setup choices to confirm with the instructor or document when done: project folder (guide: `C:\LoongIDE\RThread6`; ours: `E:\loongson1B_example\RThread6`), RT-Thread components (see discrepancy 12; none are needed by this code), serial port (guide COM5; ours COM3 at 115200).

## 5.4 Guide Section 8 (LwIP network transmission) - offline analysis, 2026-11-09

Status: **read from sources / PREDICTED**. Guide text 8.1-8.2.15 read in full (Listings 10-1 to 10-7); the LwIP sources in `D:\LoongIDE\Template\Libraries\lwip-1.4.1` and `Template.ini` were checked against it. Nothing built or run. Figures 8.1-8.12 were not examined.

Guide structure: one project (named `example_9_1` in the guide, saved under `\loongson1B_training_example\`; our name will be `example_8_lwip` in `E:\loongson1B_example`) with `main.c`, `lwip_test.h`, `lwip_test.c`, `tcp_server.c`, `tcp_client.c`, `udp_server.c`, `udp_client.c`. Exactly one of `TEST_TCP_SERVER / TEST_TCP_CLIENT / TEST_UDP_SERVER / TEST_UDP_CLIENT` is set to 1 for each test; the header ships with only `TEST_TCP_CLIENT 1`. Board address 192.168.1.123 (`gmac0_ip` in `main.c`), PC 192.168.1.111, mask 255.255.255.0, ports TCP 9060 (board server) / 9061 (PC server), UDP 9062 (board server) / 9063 (PC server).

Verified against the sources:
- Guide step 8.2.9 (optional `printk` lines at "line 509" and "line 516" of `lwIP-1.4.1/port/ls1x_ethernetif.c`): in the template file, original line 509 is the closing brace of the `if (ip0 != NULL)` block and original line 515 is the closing brace of the `else` block, so inserting at 509 and then at 516 (after the first insertion shifted the file) places the two prints inside the right blocks. The guide's line numbers are correct for this template.
- The default address in `ls1x_initialize_lwip` is 192.168.1.123 when `ip0 == NULL`; with `ip0` given, the gateway is `ip0[0].ip0[1].ip0[2].1`.
- `get_clock_ticks`, `DEFAULT_THREAD_STACKSIZE`, `DEFAULT_THREAD_PRIO`, `sys_thread_new`, `closesocket`, `ERR_CLSD`, `ls1x_initialize_lwip` all exist in the template.
- `BSP_USE_GMAC0` is commented out in the base `bsp.h`, and all network code in the guide's `main.c` sits inside `#ifdef BSP_USE_GMAC0`. `Template.ini` lists `ls1bdrv=BSP_USE_GMAC0` and `bspdefine=USE_LWIP` for the lwIP 1.4.1 library, so ticking it in the wizard should enable it. To be confirmed by looking at `include\bsp.h` of the generated project; without it the program prints only the welcome line.

Defects and risks in the guide code (reading, not run):
1. `tcp_server.c`: `accept(..., (socklen_t)&addrlen)` casts the pointer to an integer type where a `socklen_t *` is expected (compiler warning; works on this 32-bit target).
2. `udp_server.c` and `udp_client.c`: `socklen_t fromlen;` is never initialised, but LwIP 1.4.1 `lwip_recvfrom` reads `*fromlen` (`api\sockets.c` lines 685-689: it clamps and then copies `*fromlen` bytes). The server's `sendto(..., &from, fromlen)` echo may therefore go to a wrong or incomplete address depending on stack garbage. Expected failure mode: the UDP server receives but does not echo correctly. Check on the board; the fix would be `fromlen = sizeof(from)` before each call.
3. `udp_client.c` passes `MSG_WAITALL` to `recvfrom`; in this LwIP version that flag is marked "Unimplemented" (`include\lwip\sockets.h`), so the guide's comment about it is wrong. The call still blocks, so the client waits for a datagram from the PC after every send; the test needs the PC side to send once per iteration (5 times) or the client stays blocked after its first send. The TCP client behaves the same way (blocking `recv` after each send).
4. `tcp_client.c`: a failed `connect` returns permanently (no retry), so the PC-side TCP server must already be listening before the board starts.
5. Guide text issues: wrong NetAssist protocol for the UDP tests ("TCP Server" / "TCP UDP", see discrepancy 5); step 8.2.11 does not list the PC address 192.168.1.111 (discrepancy 6); `lwip_test.h` guard bug (discrepancy 4: `#endif` right after `#define _LWIP_TEST_H`); figure numbers are inconsistent (for example "Figure 8.5" for the Network Connections list, "Figure 9.6" for 8.6); the UDP tests give no remote host for the PC side.

PC set-up done on 2026-11-10 by the student, verified afterwards: Ethernet adapter `Killer E2600`, link `Up` 100 Mbps, address set manually to `192.168.1.111`, prefix length 24 (255.255.255.0), **no gateway and no DNS** (deviation from guide 8.2.11, which gives gateway 192.168.1.1: with a direct PC-to-board cable a second default route is unnecessary and could take over internet traffic; the only default route stays on Wi-Fi `10.4.0.1`, metric 0). Wi-Fi was left enabled (guide: disable other interfaces; not needed for a direct link). A ping to `192.168.1.123` gets no reply yet, which is expected: the board runs its Linux, not the LwIP program.

Project (2026-11-10): `E:\loongson1B_example\example_8_lwip`, created with the New Project Wizard (LS1B200, SDE Lite 4.9.2, RT-Thread, component lwIP 1.4.1 ticked; the summary page showed "Used components count: 0" although the box was ticked - the generated project contains `lwIP-1.4.1`, `bsp.h` has `BSP_USE_GMAC0` enabled and `USE_LWIP` defined, and the include paths contain `lwIP-1.4.1`, so the count is a display quirk, same as guide Figure 7.5). The seven source parts of the guide (Listings 10-1 to 10-7) were extracted unchanged from the guide into the project (`main.c` with the template `#include` lines, `lwip_test.h/.c`, `tcp_server.c`, `tcp_client.c`, `udp_server.c`, `udp_client.c`; originals in `evidence\example_8_lwip\guide_original\`) and the six new files were added with `Project -> Add Files to Project` (`udp_server.c` needed a second attempt). `DownloadByNet=0` (no PMON network download). IDE build: `Errors: 0`, `Warnings: 41`, 941.12 KB; `example_8_lwip.exe` SHA-256 `823481d8b6b9ca718ef4264ad15fc1b7bfa97531fe2fc3dd9e7be6a75f9f368b`, `.bin` `e6fd863fd6b8c641dede3dd5937f4d4bc348330950f98d3cdc6670f9c47a025b`, text 401756, data 5760, bss 64536. Warnings in our own files (log `own_files_compile_log.txt`, only the active test file is compiled): `tcp_client.c` implicit declarations of `memset` and `strlen` (no `<string.h>`); the other warnings come from `RTT4\include\libc\libc_ioctl.h` (macros `_IO`, `_IOW`, `_IOR`, `FIONREAD`, `FIONBIO` redefined) and vendor files.
IDE quirk found on 2026-11-10 (important for every test of this section): after `lwip_test.h` was changed (`TEST_TCP_SERVER` 1) the IDE build printed `make.exe: Nothing to be done for 'all'` and the executable still contained `tcp_client_init` instead of `tcp_server_init` (checked with `nm`). Cause: the IDE-generated dependency files use `-MT` with the `.d` name, e.g. `main.d: ../main.c ... ../lwip_test.h`, so headers are dependencies of the `.d` files, not of the `.o` files, and a header change never triggers recompilation. Workaround used here: touch (update the modification time of) the six `.c` files after every header change, or use `Project -> Make Clean` before `Compile & Build`. Always check the built test with `nm` before running it on the board.
PC-side peer: `tools\net_peer.py` (replaces NetAssist; modes `tcp-client`, `tcp-server`, `udp-client`, `udp-server`, `selftest`); the loopback self-test of all four modes passed (`evidence\example_8_lwip\net_peer_selftest.log`). Test order and `lwip_test.h` settings (one macro = 1, the others 0, as in guide 8.2.12-8.2.15) are recorded below as each test is run; the copy of the header used for each test is saved as `lwip_test.h.<TEST>` in `evidence\example_8_lwip\`.

**Test 1 - TCP server (guide 8.2.12), MEASURED 2026-11-10.** `lwip_test.h`: `TEST_TCP_SERVER 1`, others 0 (`evidence\example_8_lwip\lwip_test.h.TCP_SERVER`); binary checked with `nm` (contains `tcp_server_init`); `example_8_lwip.exe` SHA-256 `037811e8b76e1f519926fd8fd6a198df29117173946dd29d96712cb5b704a5ea`, `.bin` `5c7b5cd0e66e38a66414c295f9cc982086370ffff32939eb17429d99ae6cdb95`. Board console (PuTTY, COM7): `detect phy address == 17`, `GMAC0: SPEED=100M, DUPLEX=FULL, LINK=UP. AN=YES.`, `GMAC0 controller initialized.`, `succeed to create sock_fd!`, `succeed to bind ip!`, `succeed to listen!`, `waiting for client access!` (screenshots `example_8_tcp_server_putty_waiting.png`, `example_8_tcp_server_ide_running.png`). PC: `ping 192.168.1.123` 3 sent, 3 received, 0% loss, 0-2 ms; `tools\net_peer.py tcp-client` (log `test_TCP_SERVER_pc_peer.log`): connected from `192.168.1.111:60116` to `192.168.1.123:9060`, 5 messages `hello from PC 1..5` sent and each echoed back unchanged. Board console afterwards: `client incoming succeed!`, `RECEIVE: hello from PC 1` / `SEND: hello from PC 1` ... through 5, then `disconnect client succeed!` and `waiting for client access!` again (console log snapshot `console_log_snapshot_after_TCP_SERVER_test.log`; the PuTTY log has no timestamps and also contains the Section 7 output). Result: the guide's TCP server works as designed; the guide's expected behaviour (data received on the board console) is met, with an echo as bonus. The code-reading risk about `accept(..., (socklen_t)&addrlen)` (cast of a pointer) did not matter on this target.

Firewall finding (2026-11-10, before test 2): the first attempt of the TCP client test failed because the PC's Windows Firewall dropped the board's incoming connection. The Ethernet connection is an "Unidentified network" (direct cable, no gateway), which Windows always classes as `Public` and which cannot be switched to `Private`; the PC had two inbound `Block` rules for `python.exe` (TCP and UDP, `Public`, program `...Python313\python.exe`) that Windows creates when its firewall prompt is dismissed, and a Block rule overrides any Allow rule. Symptom: the PC server was idle and listening, the board console stopped after `succeed to create sock_fd!` (no `failed to connect` message because the connection attempt just waited). Test 1 (board as server) was not affected because the PC initiates that connection. Fix done by the student in an administrator PowerShell : the two `python.exe` Block rules were **disabled** (not deleted) and two narrow Allow rules were added: `Course lab Loongson TCP 9061` and `Course lab Loongson UDP 9063` (inbound, only from `192.168.1.123`, only interface `Ethernet`, profile `Public`). These rules must be removed after the tests: `Remove-NetFirewallRule -DisplayName 'Course lab Loongson*'` and `Get-NetFirewallRule -DisplayName 'python.exe' | Enable-NetFirewallRule`. A second symptom to remember: a PC socket bound to `192.168.1.111` goes into `FIN_WAIT_1` and stops listening when the Ethernet link drops (for example when the board is reset), so the PC peer must be started when the link is up (check with `Get-NetTCPConnection -LocalPort 9061`, state `Listen`).

**Test 2 - TCP client (guide 8.2.13), MEASURED 2026-11-10.** `lwip_test.h`: `TEST_TCP_CLIENT 1`, others 0 (`evidence\example_8_lwip\lwip_test.h.TCP_CLIENT`); binary checked with `nm` (contains `tcp_client_init`); `.exe` SHA-256 `823481d8b6b9ca718ef4264ad15fc1b7bfa97531fe2fc3dd9e7be6a75f9f368b`, `.bin` `e6fd863fd6b8c641dede3dd5937f4d4bc348330950f98d3cdc6670f9c47a025b` (identical to the first build, which had the guide's default header). PC: `tools\net_peer.py tcp-server` listening on `192.168.1.111:9061` (log `test_TCP_CLIENT_pc_peer.log`). Board console (screenshots `example_8_tcp_client_putty_output.png`, `example_8_tcp_client_ide_running.png`): `succeed to create sock_fd!`, `succeed to connect to server!`, then five times `SEND: client ticks = N.` / `RECEIVE: reply K from PC` with N = 4560, 4666, 4774, 4880, 4986 and K = 1..5, then `tcp_client_thread stop!`. PC log: board connected from `192.168.1.123:49153`; received `client ticks = 4560.`, `4666.`, `4774.`, `4880.`, `4986.`; sent `reply 1..5 from PC`; board closed the connection. Both sides show the same five values. Result: the guide's TCP client works as designed; as predicted from the code, the board blocks in `recv` after every send and continues only when the PC replies (the PC peer replied to each message).

**Test 3 - UDP server (guide 8.2.14), MEASURED 2026-11-10.** `lwip_test.h`: `TEST_UDP_SERVER 1`, others 0 (`evidence\example_8_lwip\lwip_test.h.UDP_SERVER`); binary checked with `nm` (contains `udp_server_init`); build `Errors: 0`, `Warnings: 7`, 940.61 KB; `.exe` SHA-256 `85bd5f018658016601b7ac95e09967e9e87b0fc7d9305e0f22e6937a5cea10a6`, `.bin` `8b516e85233fc3fdb53bad9c362655cdf5c5fd20f69ac4ccbcd3afe3beeda8d8`. Board console (screenshots `example_8_udp_server_putty_bind.png`, `example_8_udp_server_ide_running.png`): `succeed to create sock_fd!`, `succeed to bind ip!` (the UDP server has no listen step). PC: `tools\net_peer.py udp-client` from `192.168.1.111:9062` to `192.168.1.123:9062` (log `test_UDP_SERVER_pc_peer.log`): 5 datagrams `udp hello from PC 1..5` sent, each echoed back with identical content from `192.168.1.123:9062`. Board console afterwards: five times `RECEIVE: udp hello from PC N` / `SEND: udp hello from PC N` (snapshot `console_log_snapshot_after_UDP_SERVER_test.log`). Result: the guide's UDP server works as designed. The code-reading risk about the uninitialised `socklen_t fromlen` passed to `recvfrom` (section 5.4, risk 2) did **not** show up in this run: all five replies went to the correct address and port. That does not prove the code is correct (the value was whatever was on the stack); it only shows the failure did not occur here. No firewall rule was needed because the PC sends first and the replies are return traffic of an outgoing UDP flow.

**Test 4 - UDP client (guide 8.2.15), MEASURED 2026-11-10, with a PC-side finding.** `lwip_test.h`: `TEST_UDP_CLIENT 1`, others 0 (`evidence\example_8_lwip\lwip_test.h.UDP_CLIENT`); binary checked with `nm` (contains `udp_client_init`); `.exe` SHA-256 `08fa4614330d5d48624a109defa9c6ce8c6ad2d1901ebfa5f89b17ee74263a3b`, `.bin` `e0412110e4b758d0b802be0b07746e9abf5c4b107b5f020d842e36b26389a0d4`. Board console (snapshot `console_log_snapshot_after_UDP_CLIENT_test.log`; final screen in `example_8_udp_client_putty_final.png`): `succeed to create sock_fd!`, then five times `SEND: client ticks = N.` with N = 1805, 245106, 245248, 389904, 519996, each followed by `RECEIVE: ...` of a PC datagram, and finally `udp_client_thread stop!`.

What happened (two boots with the PC listener already running, about 11:34 with `client ticks = 2005.` and 11:38 with `client ticks = 1805.`; an earlier boot at about 11:30 happened before the listener existed and is not counted; screenshots `example_8_udp_client_putty_first_send_2005.png`, `example_8_udp_client_putty_first_send_1805.png`): the **first** datagram after a board boot (`client ticks = 1805.`, source `192.168.1.123:49153`, destination `192.168.1.111:9063`) never reached the Python listener although the Allow rule `UDP 9063` existed, the listener was bound and the firewall block rules were disabled. The board then waits forever in `recvfrom()` (the guide code sends once per received reply and has no retry), so the test stalls. Capture with `pktmon` (`udp_client_capture.etl/.txt/.pcapng`, run by the student in an administrator PowerShell; filter `192.168.1.123`): the datagram **arrived at the PC's network adapter** and was then **dropped inside the PC's IP stack** with the pktmon drop reasons `INET: receive inspection`, `INET: transport endpoint was not found` and `Inspection drop` (all reported at pktmon component 81). The frame itself is correct (`udp_client_capture.pcapng` parsed with `tools\pcapng_udp_check.py`: Ethernet destination = PC MAC `08-8F-C3-82-AC-23`, IPv4 header checksum valid, UDP checksum valid, TTL 255, payload `client ticks = 1805.`, 20 data bytes), so the fault is not on the board. Controlled probes on the same board run (board still blocked in `recvfrom`; PC sends a datagram to the board's source port 49153, which makes the board send its next datagram): round 1 (PC sends from its own port 9063, reply is return traffic) received; round 2 (wildcard bind `0.0.0.0:9063`) received; round 3, after 130 s (UDP flow state expired), specific bind `192.168.1.111:9063` with an unsolicited reply: **received**; round 4, after another 130 s, wildcard bind: **received**; the 5th reply (`reply 5 from PC`) ended the board loop. Conclusion: the firewall Allow rule works for unsolicited datagrams; only the first datagram after a board boot is lost on the PC side (reproduced twice). The cause of that drop is **not determined** (candidates: the PC neighbour/ARP state right after the board's boot, the receive-inspection path of the Killer adapter driver; neither was tested). Workaround used and implemented in `tools\net_peer.py udp-server --nudge N`: the PC sends a short datagram to the board's source port 49153 while nothing has arrived; the board then sends its next datagram, which arrives. The board's `RECEIVE:` line therefore shows PC datagrams (`trigger from ...`, last one `reply 5 from PC`), not `reply 1..5` as the plain peer would send; the PC received the board's datagrams `client ticks = 245106.`, `245248.`, `389904.`, `519996.` (probe output, `11:42` to `11:47`) but not `1805.`. Result: the board's UDP client code works as designed (sends, receives, finishes after five loops); a complete clean pass with the plain peer and without the nudge was **not** achieved.

![Figure 8.1 - UDP client test, final board console: five SEND lines, the PC datagrams received, and udp_client_thread stop!](evidence/screenshots/example_8_udp_client_putty_final.png)

*Figure 8.1 - UDP client test (guide 8.2.15), final PuTTY screen: SEND client ticks = 1805, 245106, 245248, 389904, 519996; RECEIVE lines with the PC datagrams; the last one `reply 5 from PC`; then `udp_client_thread stop!`.*

Clean-up after Section 8 (checked on 2026-11-10 with read-only commands): the two temporary Allow rules `Course lab Loongson TCP 9061` and `UDP 9063` were removed and the two inbound `python.exe` Block rules were enabled again (state before the tests restored); `pktmon` is not running (its filters were removed); no Python test process is left. The Ethernet adapter was set back to `Automatic (DHCP)` by the student on 2026-11-10 (verified: link-local address, default route still on Wi-Fi); the manual address `192.168.1.111/24` has to be set again for any further network test.

Missing for the hardware test: the NetAssist tool (not provided), Ethernet cable, serial console, board. PC-side Python peers (TCP server and client, UDP echo) can replace NetAssist for repeatable tests; not written yet.

## 5.5 Guide Section 9 (Linux port) - offline analysis, 2026-11-10

Status: **read from the guide**; nothing installed, built or flashed. Guide text 9.1-9.3 read in full; figures 9.1-9.61 not examined.

Workflow in the guide: install Tftpd32 4.50 on Windows (9.2.1); install VMware Workstation Pro 16.2.0 (9.2.2); install Ubuntu 16.04.7 desktop amd64 in a VM, 20 GB disk (9.2.3); share `D:\loongson_linux_file` with the VM (9.2.4); unpack the cross-compiler `gcc-4.3-ls232.tar.gz` to `/` (gives `/opt/gcc-4.3-ls232`, 9.2.5); build the Linux 3.0 kernel from `linux.zip` with `make ARCH=mips CROSS_COMPILE=mipsel-linux-` to get `vmlinux` (9.2.6); build the root file system from `buildroot.tar.gz` the same way (9.2.7); copy `vmlinux` and `rootfs.ext2` to the Tftpd32 directory (9.2.8); connect the board (5 V, Ethernet, Micro-USB) and open PuTTY at 115200 (9.2.9); at the `PMON>` prompt run `mtd_erase /dev/mtd0`, `mtd_erase /dev/mtd1`, `ifconfig syn0 192.168.0.11`, `ping`, `devcp tftp://<host>/vmlinux /dev/mtd0`, `devcp tftp://<host>/rootfs.ext2 /dev/mtd1`, `set al /dev/mtd0`, `set append "root=/dev/mtdblock1 console=ttyS5,115200 rdinit=/sbin/init rootfstype=ext2"`, `reboot` (9.2.10); then compile own C programs with `mipsel-linux-gcc` and run them on the board via `tftp -g` (9.2.11).

What we do **not** have (checked): the archive `materials\related software.zip` contains only the four installers already used (SDE Lite, LoongIDE, MSYS, PuTTY). Missing: `gcc-4.3-ls232.tar(.gz)`, `linux.zip`, `buildroot.tar.gz` (instructor), Tftpd32 4.50, VMware Workstation Pro 16.2.0 (the guide gives a Baidu link with extraction code 8888; trial is 30 days), the Ubuntu 16.04.7 ISO (the guide points to `releases.ubuntu.com`). Downloads need the user's go-ahead; none was done.

Safety: the destructive commands of 9.2.10 (`mtd_erase`, `devcp`, `set`) overwrite the Linux kernel and root file system that are already in the board's flash (the board boots `Welcome to Buildroot`, console `ttyS5`, root on `mtdblock1`, see 2.4); a failed build would leave the board without Linux until it is re-flashed over the network from PMON. They must not be run without the instructor's approval (standing rule of this project).

Points to clarify or discrepancies (in addition to the numbered table above):
- Console port: 9.2.9 expects two virtual serial ports from the Micro-USB cable (COM3/COM4) and uses COM3 for PuTTY, while Section 7 uses a separate USB-serial cable on UART5. On the board `LXLINK-0102` the FTDI ports `COM4`/`COM5` existed; `COM5` (channel B) gave no data after an Enter; `COM4` (channel A) was not tested. On the board `LXLINK-0103` Windows shows the two FTDI converters but no COM ports.
- Host address: `ping 192.168.0.10` and `devcp tftp://192.168.0.10/vmlinux`, but `devcp tftp://192.168.0.104/rootfs.ext2`; the Tftpd32 step first sets the interface to `127.0.0.1`, later to `192.168.0.104` (discrepancy 7). The board address `192.168.0.11` is in a different subnet than the 192.168.1.x addresses of Section 8.
- `output/image` (text) versus `output/images` (figure) and `rootfs.ext2` versus `rootfs.yaffs2` (discrepancy 1); the cross-compiler archive name (discrepancy 3).
- Step numbering and figure numbering are repeated or out of order in 9.2.8 (for example "Figure 9.53" twice, "Figure 9.55" cited for the Tftpd32 dialog).
- The PC firewall must be switched off for `ping` (9.2.10 note); this is a security setting and has to be done by the user.

---

## 6. Other deliverables
- `docs\konspekt_MIPS32.md` - study notes (English), including the RISC vs CISC answer.
- `docs\teacher_request.md` - missing materials and questions.
- `PROJECT_STATUS.md` - short status table.
- Backup of the workspace: `E:\LoongSetup\backup\loongson1B_example_2026-11-02.zip` (before the 2026-11-03 work).

---

## 7. Run sheets for board measurements (to be filled)

Common set-up: USB-C on `DEBUG`, 5 V adapter on; LoongIDE `Debug -> Options -> Main`: "Fast Download ... Over Network" **unchecked**. After each session press **Stop**; the project has "reset after debug stop", so the board returns to Linux. Do not run Linux commands that write to flash.

### 7.1 `example_5_asm`
1. Open `E:\loongson1B_example\example_5_asm\example_5_asm.lxp`; `Project -> Compile & Build` (expect 0 errors).
2. `Debug -> Run`. Set a breakpoint on the instruction after `sw t0,0(t5)` (or on `jr ra`).
3. Continue to that breakpoint. Record (CPU Registers window): 

| Register | Expected | Measured (2026-11-03) |
|----------|----------|----------|
| t0 | 0xFFFE0001 | 0xfffe0001 - MEASURED |
| t1 | 0x0001FFFF | 0x0001ffff - MEASURED |
| t2 | 0xFFFFFFFF | 0xffffffff - MEASURED |
| t3 | 0x0001FFFD | 0x0001fffd - MEASURED |
| t4 | 0x0001FFFE | 0x0001fffe - MEASURED |
| t5 | 0x80030000 | 0x80030000 - MEASURED |
| memory @0x80030000 (little-endian bytes) | 01 00 FE FF | 01 00 FE FF - MEASURED on the second attempt (first attempt failed, see 3.4) |

Step 5 (retry of the memory read): reset the board, wait about 10 s, `Debug -> Run`, stop at line 47, then `View Memory` with address `0x80030000`, type `unsigned int`, count 1 (a single-word read instead of 64 bytes). If it fails again: unplug and re-plug the USB-C cable (or use another USB port), then repeat. Record the console text in either case.

### 7.2 `example_7_2` (cache experiment)
1. Open `E:\loongson1B_example\example_7_2\example_7_2.lxp`; Build (expect 0 errors, `.bin` hash `b3d8...81ed` if the IDE output is compared).
2. `Debug -> Run`. Use the stored breakpoints at `bsp_start.S` line 26 and 34; add one after `lw v1` and one after `lw a2`; use **Continue** between them (do not single-step through `cache`).
3. Do **not** continue past the last instruction (the `s0` bug).
4. Record:

| Stop point | Registers | Expected (prediction) | Measured |
|------------|-----------|-----------------------|----------|
| line 26 (halts before `li a0`) | a0 | 0 (not yet loaded) | 0x00000000 (a1=0x80001fe0, v1=0x80204b38) |
| line 34 (after `lw a1`) | a0, a1 | a0=0xA5A55A5A, a1 != 0xA5A55A5A | a0=0xA5A55A5A, a1=0x02C02021 |
| line 43 (after `lw v1`) | v1 | 0xA5A55A5A | 0xA5A55A5A |
| line 50 (after `lw a2`) | a2 | 0x12345678 | **0xFFFE0001 (mismatch)** |
| memory @0xA0020000 | | 0xA5A55A5A | 0xA5A55A5A |
| memory @0xA0030000 | | 0x12345678 | **0xFFFE0001 (mismatch)** |

Run sheet note: the original row "after `li a0`" assumed the breakpoint stops after the instruction; it stops before. Full analysis: section 4.6.

### 7.3 Evidence checklist
For each run keep: screenshot of the registers/memory, the IDE console text, and the SHA-256 of the built `.exe` (`Get-FileHash`).

---

## 8. File inventory
| Path | Content |
|------|---------|
| `E:\loongson1B_example\example_4_1` | project 4 (C) |
| `E:\loongson1B_example\example_5_asm` | project 5 (odd-number sum) |
| `E:\loongson1B_example\example_7_2` | working copy of the instructor's cache project |
| `E:\LoongSetup\teacher_files\example_7_2` | untouched extraction of the zip |
| `E:\LoongSetup\builds\...` | command-line builds used for verification |
| `E:\Projects\Compherisive design\evidence\...` | evidence (hashes, disassembly, logs, project files) |
| `E:\Projects\Compherisive design\tools\build_asm_project.sh` | verification build script |
| `E:\Projects\Compherisive design\docs\` | `teacher_request.md`, `konspekt_MIPS32.md` |
| `E:\Projects\Compherisive design\materials\` | guide, MIPS32 PDFs, slides, "related software" archive |
| `E:\Projects\Compherisive design\README.md` | folder map and status |
