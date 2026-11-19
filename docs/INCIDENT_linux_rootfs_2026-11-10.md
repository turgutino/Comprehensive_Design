# Incident: board Linux no longer boots (kernel panic), 2026-11-10

Message that can be sent to the instructor (English; Chinese/Azerbaijani translation is up to the student).

## What happens

After power-up the board starts PMON and the Linux kernel, then stops with:

```
VFS: Mounted root (ext2 filesystem) readonly on device 31:1.
/sbin/init: error while loading shared libraries: /lib/libm.so.6: cannot read file data: Invalid argument
Kernel panic - not syncing: Attempted to kill init!
```

Seen 3 times in a row (about 15:17, 15:19 and after a 2-minute power-off with the USB-C DEBUG and Ethernet cables removed). Screenshot: `evidence\section9_hello\linux_kernel_panic_libm_15-2x.png`. Serial console: CH340 on the DB9 labelled UART5, COM7, 115200 8N1, flow control None.

## Last known good state

Linux booted normally and showed `buildroot login:` on this board (board `LXLINK-0103`) at about 14:20 on 2026-11-10 (PuTTY screenshot of that boot, `Welcome to Buildroot`, console ttyS5).

## What was done between the good boot and the panic (everything, nothing hidden)

- Programs were downloaded to RAM over EJTAG with LoongIDE and run (`example_6_led`: UART3 + 8 GPIO outputs, GPIO 9, 11, 15, 17, 20, 22, 6, 7, all driven as outputs; an 8-LED add-on board was connected to the headers and powered from the board header). Three debug starts printed `wait for pracc of LS232(1B/1C) fail` (the same message as earlier, non-fatal in Section 5).
- Several power cycles of the 5 V adapter and several debug Stops (the board resets after Stop).
- The CH340 serial cable was moved between DB9 connectors; the LED add-on board wires were removed before the panic appeared.
- **No flash commands were used**: no `mtd_erase`, `devcp`, `set`/`unset`, "Program NOR Flash". The root file system is mounted read-only.

## Not known

- Whether the root file system in flash is damaged, or the flash read fails (supply, flash chip, driver).
- Whether anything in the RAM programs (GPIO output on pins that may be shared with the flash/SPI interface) could play a role. This is a **hypothesis only**; the pin multiplexing of GPIO 6, 7, 20, 22 was not checked.

## Replacement board (15:40)

The board was swapped. The new board stays at the `PMON>` prompt after every power-up (two power cycles, no key pressed). `env` (read-only) shows no `al` / `append` variables and `ethaddr = ff:ff:ff:ff:ff:ff` (file `evidence\section9_hello\pmon_env_new_board_2026-11-10.txt`). Nothing was changed on this board. It is not known whether a kernel and a root file system are in its flash. (`LXLINK` number of the debug probe on this board not yet read.)

## What is needed

The board's Linux image must be restored, which needs the instructor's files (`vmlinux`, `rootfs.ext2`) and the PMON flash commands of guide 9.2.10 (`mtd_erase`, `devcp`, `set`). The student will **not** run these without the instructor's explicit approval.

Questions for the instructor:
1. Has this panic happened on other boards? Is there a known cause?
2. May we restore the rootfs, and can you send `vmlinux` and `rootfs.ext2` (or tell us where to download them)?
3. Is there a spare board with a working Linux image for the Section 9 demonstration?
