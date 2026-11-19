/*
 * bsp_start.S - cache_probe
 *
 * Own design (course "own design" task). Based on the instructor's cache
 * experiment (example_7_2), extended to tell apart two explanations of the
 * observed result (see REPORT.md section 4.6.1):
 *   H1: an uncached store to 0xA0030000 never changes the word that both the
 *       uncached and the cached load see.
 *   H2: the store reaches RAM, but a stale valid D-cache line for physical
 *       0x00030000 is served to the cached load (and to the debugger).
 *
 * Phase A uses physical 0x00030000 (the word of the instructor's test),
 * phase B uses physical 0x00040000 (control, no earlier access).
 * All results stay in registers; stop at the last breakpoint and read them.
 * s0 holds the return address and is never overwritten.
 *
 * Result registers (physical 0x30000 = phase A, 0x40000 = phase B):
 *   t0  uncached load before the store            (A)
 *   t1  cached   load before the store            (A)
 *   t2  uncached load after  store + sync         (A)
 *   t3  cached   load after  store + sync         (A)
 *   t4  cached   load after  cache 0x11 (hit invalidate D) (A)
 *   t5  uncached load after  the invalidate       (A)
 *   t6  uncached load before the store            (B)
 *   t7  uncached load after  store + sync         (B)
 *   t8  cached   load after  store + sync         (B)
 *   a0  stored value 0x12345678
 */

#include "regdef.h"
#include "cpu.h"
#include "asm.h"

//-----------------------------------------------------------------------------

    .text

FRAME(bsp_start,sp,0,ra)
    .set noreorder

    move    s0, ra                  /* return address (kept untouched) */

    li      a0, 0x12345678          /* value to store */

    /* ---------------- phase A: physical 0x00030000 ---------------- */
    lui     a1, 0xa003              /* a1 = 0xA0030000 (kseg1, uncached) */
    lui     a2, 0x8003              /* a2 = 0x80030000 (kseg0, cached)   */

    lw      t0, 0(a1)               /* t0: uncached load, before */
    nop
    lw      t1, 0(a2)               /* t1: cached load, before (fills the line) */
    nop

    sw      a0, 0(a1)               /* uncached store */
    sync                            /* drain the write buffer */
    nop

    lw      t2, 0(a1)               /* t2: uncached load, after */
    nop
    lw      t3, 0(a2)               /* t3: cached load, after */
    nop

    cache   0x11, 0(a2)             /* D-cache, Hit Invalidate (0b10001) */
    nop
    nop
    nop

    lw      t4, 0(a2)               /* t4: cached load after invalidate */
    nop
    lw      t5, 0(a1)               /* t5: uncached load after invalidate */
    nop

    /* ---------------- phase B: physical 0x00040000 (control) ------ */
    lui     a1, 0xa004              /* a1 = 0xA0040000 */
    lui     a2, 0x8004              /* a2 = 0x80040000 */

    lw      t6, 0(a1)               /* t6: uncached load, before */
    nop

    sw      a0, 0(a1)               /* uncached store */
    sync
    nop

    lw      t7, 0(a1)               /* t7: uncached load, after */
    nop
    lw      t8, 0(a2)               /* t8: cached load, after */
    nop

    move    ra, s0                  /* <-- last breakpoint: read t0..t8 here */
    j       ra
    nop

    .set reorder
ENDFRAME(bsp_start)

//-----------------------------------------------------------------------------

/*
 * @@ END
 */
