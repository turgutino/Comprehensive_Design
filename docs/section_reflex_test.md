# Own Design - Reflex Test (section for the report)

Program: `reflex_test`, Loongson 1B (LS1B200), bare-metal C, LoongIDE 1.1 /
SDE Lite 4.9.2, built with the BSP of the course (no new driver, no RTOS).

Source: `E:\loongson1B_example\reflex_test\main.c` (338 lines).
Copy: `evidence/reflex_test/main.c`. Binary measured on the board:
`reflex_test.exe` SHA-256 `53ada7bb…0a0a5f7f`, `.bin` `c59b5f9d…cebcf3`
(size: text 55 000, data 3 600, bss 18 120).

## 1. What it does

The board measures the reaction time of the operator over the serial console.
`R` arms one measurement; the board waits a random 1-3 s so that the cue cannot
be anticipated, prints `>>>> GO! <<<<`, and the milliseconds between the cue and
the first character typed are the result. `B` shows the best result, `A` the
average of the last ten, `H` the command list. A key pressed before the cue is a
false start and cancels the measurement; no key within 5 s is a timeout.

Why this design: it exercises three layers of the platform at once - the 1 ms
tick of the BSP (`get_clock_ticks`), the polled UART driver (both UART3 and
UART5 are polled with `timeout = 0`, so the loop never blocks), and a small
state machine with no dynamic memory and no `printf`.

## 2. Commands

| Command | Effect |
|---|---|
| `R` | arm a measurement; the board waits 1-3 s, then prints the cue |
| any character after the cue | answer; prints `reaction: N ms` |
| `B` | best result so far |
| `A` | average of the last 10 results |
| `H` | command list |

## 3. Measured results (board, 2026-11-19)

Six measurements, in the order in which they were taken:

| # | Result | |
|---|---|---|
| 1 | 268 ms | |
| 2 | 384 ms | |
| 3 | 240 ms | |
| 4 | 33 ms | below the human limit, see §5 |
| 5 | 420 ms | |
| 6 | 284 ms | |

Average of the six: **271 ms**. Best: **33 ms** (see §5).
The printed running average (268, 326, 297, 231, 269, 271) and the best value
were checked against the arithmetic after every step and agree exactly.

Behaviour predicted before the run and then observed: banner; `H`, `B`, `A` on an
empty statistics; `get ready ...` followed by the cue after 1-3 s; `FALSE START`
for a key pressed during the wait; `timeout - no key within 5 s`.

## 4. Reproducibility

| Build | exe SHA-256 | bin SHA-256 |
|---|---|---|
| command line | `2cbdf1d0…7f620b` | `4320d9b9…a9df3` |
| LoongIDE (`Make Clean` + `Compile & Build`) | `2cbdf1d0…7f620b` | `4320d9b9…a9df3` |

The IDE and the command line produce byte-identical files; a clean rebuild
reproduces the same `.bin`. 0 errors, 22 warnings, all of them in vendor/BSP
files (`sscanf.c`, `ns16550.c`, `strtod.c`, ...), none in `main.c`.

## 5. Limitations, stated honestly

1. **33 ms is not a reaction time.** The human limit is about 100 ms, so this
   value is an anticipation: the key was already pressed when the cue appeared.
   It is kept in the log and shown as the "best" result, which is why the best
   value must be read together with this note.
2. The measured value contains the terminal and UART latency in addition to the
   human reaction, so it is systematically larger than a laboratory reaction
   time. The resolution is 1 ms, the useful accuracy is far coarser.
3. **A display defect was found and is reported, not hidden.** In an earlier
   build that wrote one character per UART call, two of six printed numbers
   contained a stray character (`34a7` instead of `347`, `25=1` instead of
   `251`). The output path was then changed to assemble each line in RAM and
   send it with a single `ls1x_uart_write`; after that, 8 of 8 printed values
   were clean. That is consistent with the per-character write being the cause,
   but 8 values are not a proof and this is stated as an indication only.
4. In the measured build the last line of the `H` help text is **truncated**
   (line buffer 192 characters, help block 220). The source was corrected
   afterwards (`LINE_MAX` 320) and a corrected binary was built and hashed, but
   it was deliberately not loaded on the board: the defect touches only the help
   screen and no measured value. The corrected build was therefore not verified
   on hardware.
5. Nothing is written to flash: the statistics live in RAM and are lost on a
   board reset. The program can be stopped at any moment without leaving the
   board in a modified state.

## 6. Evidence

| File | Content |
|---|---|
| `03_putty_banner.png` | the banner after the program was downloaded |
| `05_binaryB_fresh_start.png` | `A`/`B` on an empty statistics after a real reset (proof that the new image was running) |
| `07_binaryB1_six_measurements_and_H.png` | the six measurements, `A` = 271 ms, `B` = 33 ms, and the truncated `H` |
| `SESSION_LOG.md` | every number with the binary it came from, including the discarded attempts |
| `build_clean_before_fix.log`, `build_after_fix_incremental.log`, `build_after_line_max_fix.log` | compiler output |
| `reproducibility.md`, `sha256.txt`, `size.txt` | hashes and sizes |
| `predictions.md` | the predictions written **before** the board run |
| `_unusable_fullscreen/` | full-screen grabs that were rejected as evidence, with the reason |
