# System changes made during the lab work (2026-11-02 to 2026-11-10)

Everything that was changed on the PC, on the tools or on the board, with the current state and how to undo it.
State checked on 2026-11-10 (read-only commands). "By student" = done by the student; the Windows security and network settings were changed by the student, not by any tool.

| # | Change | Why | Current state (2026-11-10) | How to undo |
|---|--------|-----|----------------------------|-------------|
| 1 | Ethernet adapter was set to manual IPv4 `192.168.1.111`, mask `255.255.255.0`, no gateway, no DNS (by student) | Section 8 network tests | **Reverted on 2026-11-10 by the student**: `Automatic (DHCP)`; the adapter now has the automatic link-local address `169.254.x.x` (normal without a DHCP server); the only default route is still Wi-Fi | Settings > Network & internet > Ethernet > IP assignment > Edit > Automatic (DHCP) |
| 2 | Firewall: two inbound `python.exe` Block rules disabled; rules `Course lab Loongson TCP 9061` and `UDP 9063` added (by student, administrator PowerShell) | Board connections to the PC in the TCP-client and UDP-client tests | **Fully restored**: the Course lab rules were removed and the `python.exe` Block rules are enabled again (verified) | nothing to do |
| 3 | CH340 serial adapter: device uninstalled, driver package removed and re-installed with the instructor's `CH341SER.EXE` (version 4.0.2026.2, same as before), FIFO option unticked once (by student) | Serial console problem, see `REPORT.md` 1.3.1 | Driver installed and working; the active port (`COM7`) uses default port settings | not needed; the driver is required for the serial console |
| 4 | System PATH extended with `C:\mingw\bin` and `C:\msys\1.0\bin` (2026-11-02, by student, administrator PowerShell; the lower-case duplicates were added by the installer) | `make` for LoongIDE | still set | System Properties > Environment Variables (not recommended while the course runs) |
| 5 | Software installed: MSYS/MinGW (`C:\msys`, `C:\mingw`), LoongIDE 1.1 (`D:\LoongIDE`), SDE Lite 4.9.2 (inside LoongIDE), FTDI LxLink driver (CDM 21228), PuTTY 0.63 (portable, `E:\LoongSetup`) | build and debug tools | installed | Windows Apps list / delete the folders after the course |
| 6 | `pktmon` capture with a filter on `192.168.1.123` (by student, administrator PowerShell) | UDP client analysis | `pktmon` not running, filters removed | nothing to do |
| 7 | PuTTY session logging to `evidence\example_7_barber\putty_COM7_20261110_105614.log` | raw console logs | session setting only, not saved as a default | close PuTTY |
| 8 | LoongIDE projects: `DownloadByNet=0` ("Fast download over network of PMON" unticked), `SetBPAtFunc` and breakpoints set per project | no Ethernet/PMON network download | set in the project files of `example_4_1`, `example_5_asm`, `example_7_2`, `cache_probe`, `RThread6`, `example_8_lwip` | Debug > Options |
| 9 | Board: **no change to the flash or to the PMON environment**; programs were downloaded to RAM over EJTAG. After every Stop the board resets and boots its own Linux | - | unchanged (`mtd_erase`, `devcp`, `set`, "Program NOR Flash" were never used) | nothing to do |
| 10 | Files created: `E:\loongson1B_example` (projects), `E:\LoongSetup\builds` (command-line verification builds), `E:\LoongSetup\backup` (zip and file backups), `E:\Projects\Compherisive design` (reports, evidence, tools) | work and evidence | present | keep until the report is handed in |

Added 2026-11-10 (Section 6): the instructor's `AiCube-ISP-v6.96M` tool was run by the student (serial helper only, no `Download/Program`); PuTTY serial flow control changed from `XON/XOFF` to `None` (session setting only); the CH340 USB cable was re-plugged to clear `Unable to configure serial port`; the LED add-on board was wired to the board headers (GPIO 9, 11, 15, 17, 20, 22, 6, 7); no flash or PMON change.

Not changed: Windows Update settings, power plan (Balanced), Wi-Fi settings, the firewall master switch and profiles, Defender, any driver other than CH340 and FTDI.

Events that were not changes made by us: the Windows bugcheck of 2026-11-09 11:24 (`REPORT.md` 1.3.2); the ARP/UDP drop on the PC receive path (`REPORT.md` 5.4).
