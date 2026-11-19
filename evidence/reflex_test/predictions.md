# reflex_test - predictions written BEFORE the board run

Date: 2026-11-18. Binary measured: `reflex_test.exe` SHA-256 `93dadadb…75075`,
`.bin` `4320d9b9…a9df3` (see `sha256.txt`); `text 55008, data 3600, bss 17928`.

The program prints on **both** UART3 and UART5, so the banner must appear
whichever DB9 the CH340 cable is on. Time base: `get_clock_ticks()`, 1 ms.

| # | Input / situation | Predicted behaviour |
|---|---|---|
| P1 | after `Debug -> Run` + Continue | banner `=== Loongson 1B : reflex test ===` + three info lines + `> ` prompt |
| P2 | `H` | the five-line command list, then `> ` |
| P3 | `B` before any measurement | `best: no result yet` |
| P4 | `A` before any measurement | `average: no result yet` |
| P5 | `R` | `get ready ...`, then after **1000-3000 ms** the line `>>>> GO! <<<<` |
| P6 | one key typed 0.2 s after `GO` | `reaction: N ms` with **N < 1000**, then `best`, `average of 1` |
| P7 | key typed **before** `GO` (during the wait) | `FALSE START - the cue had not come yet.`, measurement cancelled |
| P8 | no key within 5 s after `GO` | `timeout - no key within 5 s` |
| P9 | unknown key (`Z`) in idle | `? type R to start a measurement, H for help` |
| P10 | three `R` in a row without answering | the wait before `GO` differs each time (random 1000-3000 ms) |
| P11 | every key typed | echoed once, on the same port |
| P12 | `B` / `A` after several results | best = smallest N seen; average over the last (at most 10) results |

Known limits, to be stated in the report: the value contains the terminal and
UART latency and human reaction; resolution is 1 ms but the useful accuracy is
far coarser. No result survives a board reset (nothing is written to flash).
