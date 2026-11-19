# reflex_test - build reproducibility

Source: `main.c` (300 lines, in this folder). Flags: the project's own makefiles
(`-mips32 -G0 -EL -msoft-float -DLS1B -DOS_NONE -O1 -g -Wall`), linker `-T ../ld.script`.

## Builds compared

| # | Where | When | exe SHA-256 | bin SHA-256 | errors | warnings | size (text) |
|---|---|---|---|---|---|---|---|
| 1 | command line, first build | 2026-11-19 | `93dadadb…75075` | `4320d9b9…a9df3` | 0 | 22 | 55008 |
| 2 | command line, `make clean` + `make all` | 2026-11-19 | `2cbdf1d0…7f620b` | `4320d9b9…a9df3` | 0 | 22 | 55008 |
| 3 | **LoongIDE** `Project -> Make Clean` + `Compile & Build` | 2026-11-19 | `2cbdf1d0…7f620b` | `4320d9b9…a9df3` | 0 | 22 | 55008 |

## Findings

1. **IDE build = command-line build, byte for byte** (both `.exe` and `.bin`), so the
   binary that will be measured on the board is the one that was verified here.
2. The `.bin` (the raw image that matters) is **identical in all three builds**, i.e.
   the build is reproducible.
3. `.exe` of build #1 differs from #2/#3 only in the embedded debug information
   (file/line tables), not in the code; the `.bin` proves it.
4. The 22 warnings are all in vendor/BSP files (`sscanf.c` 11, `ns16550.c` 4,
   `strtod.c` 2, `strcpy.c`, `strcat.c`, `aligned_malloc.c`, `irq_s.S`,
   `exception.c` 1 each) — **none in `main.c`** (0 warnings).
5. Full logs: `build.log` (505 lines), summary: `build_summary.txt`.

## Screenshots

- `01_ide_build_panel_nothing_to_do.png` — IDE panel right after the project was
  opened; it says "Nothing to be done for 'all'", i.e. nothing was compiled at
  that moment (the objects of the command-line build were already current). Kept
  for honesty, **not** used as build evidence.
- `02_ide_clean_build.png` — the real IDE build after `Make Clean`:
  `Errors: 0`, `Warnings: 22`, output 271.07 KB, 9.53 s.
