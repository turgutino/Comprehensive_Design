# reflex_test - session log (provenance of every number)

One line per run, so that no measurement is ever attributed to the wrong binary.

## Run 1 - binary A (per-character output)

- binary: `reflex_test.exe` sha256 `2cbdf1d0…7f620b`, `.bin` `4320d9b9…a9df3`
  (built 2026-11-19, IDE and command line produced the same file, see reproducibility.md)
- source: the first version of `main.c` (one `ls1x_uart_write` per character)
- loaded: LoongIDE `Debug -> Run` over EJTAG, `Debug -> Continue`
- observed:
  - banner, `H`, `B` (no result yet), `A` (no result yet) - as predicted P1-P4
  - `FALSE START` before the cue - as predicted P7
  - `timeout - no key within 5 s` - as predicted P8
  - measurements, in this order:
    | # | key typed | shown | notes |
    |---|---|---|---|
    | 1 | 6 | `34a7 ms` | **artifact**: a stray character inside the number |
    | 2 | 8 | `266 ms` | clean |
    | 3 | 3 | `289 ms` | clean |
    | 4 | 8 | `25=1 ms` | **artifact** |
    | 5 | 7 | `435 ms` | clean |
    | 6 | 8 | `421 ms` | clean |
  - `best: 251 ms`, `average of 5: 317 ms`, `average of 6: 334 ms`
    - arithmetic check: (347+266+289+251+435)/5 = 317.6 -> 317 ✔
    - (347+266+289+251+435+421)/6 = 334.8 -> 334 ✔
    - best = 251 ✔
    - the averages prove that all six measurements belong to **one continuous
      process**, i.e. the board was never reset between them, so runs 1..6 all
      come from binary A
- screenshots: `04_putty_measurements_5_6.png` (values 5 and 6),
  `_unusable_fullscreen/` (the earlier full-screen grabs, not used)

## Run 2 - binary B (one write per line)

- binary: `reflex_test.exe` sha256 `53ada7bb…0a0a5f7f`, `.bin` `c59b5f9d…cebcf3`
- source: second version of `main.c` (`lb_*` line buffer, one write per line)
- status: **not run yet**. To prove that binary B is really the one running, the
  first command after the download must be `A`, which has to answer
  `average: no result yet` (the statistics live in RAM and a reset clears them).
  If the counter continues from 6, the board was not reset and the old binary is
  still in RAM.

## Binary B1 and B2 (the second source version)

- B1: exe `53ada7bb…0a0a5f7f`, bin `c59b5f9d…cebcf3`
  - run on the board after Stop/Run: banner appeared, `A` and `B` answered
    "no result yet", which proves the RAM was cleared and B1 was running.
  - defect found on the board: the `H` help block is 220 characters, the line
    buffer was 192, so the last line was cut ("... or"). Cosmetic, but visible.
  - screenshot: `05_binaryB_fresh_start.png`
- B2: exe `aae94f50…6969e340`, bin `10a77157…05e4b5719`
  - only change: `LINE_MAX` 192 -> 320 (help block fits), bss 18120 -> 18248.
  - this is the binary used for the final measured session.

## Run 3 - binary B1 (six measurements, and the help truncation seen again)

- binary: B1, exe `53ada7bb…0a0a5f7f`, bin `c59b5f9d…cebcf3` (LINE_MAX = 192)
- measurements, in order: 268, 384, 240, 33, 420, 284 ms
  - all six values printed **clean**, no stray character
  - arithmetic check: averages 268, 326, 297, 231, 269, 271 and best 33 ms exactly
    as printed after every step
  - the value 33 ms is below the human reaction limit (~100 ms) and is therefore
    reported as an anticipation (the key was pressed before or at the cue), not
    as a measurement of reaction time
- `A` -> average of 6: 271 ms; `B` -> best: 33 ms
- `H` -> **last line truncated** ("any other key: start a measurement (R), or"),
  which proves that the running binary was still B1 (LINE_MAX 192), i.e. B2 was
  **not** loaded in this session
- screenshot: `07_binaryB1_six_measurements_and_H.png`

## Status of binary B2

- built (exe `aae94f50…6969e340`, bin `10a77157…05e4b5719`, LINE_MAX = 320)
- **not yet run on the board**; the only difference from B1 is the line buffer size,
  so the measurements above remain valid for both builds.

## Accepted limitation (decision of 2026-11-19: not repaired on the board)

The measured build (B1) truncates the last line of the `H` help text: the line
buffer is 192 characters and the help block is 220. The source was corrected
afterwards (`LINE_MAX` 320, binary B2 built and hashed), but B2 was deliberately
**not** loaded on the board: the defect is cosmetic, it touches only the help
screen, and every measured value is unaffected. The defect is therefore reported
as it is instead of being repaired behind the reader's back.
