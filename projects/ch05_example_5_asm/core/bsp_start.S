/*
 * Section 5 - assembly exercise
 *
 * Sum of consecutive odd numbers 1 + 3 + 5 + ... (the sum of the first n
 * odd numbers is n^2). The loop stops before the unsigned 32-bit range
 * would overflow, using a "remaining space" test instead of relying on an
 * overflow exception. The final sum is stored at address 0x80030000.
 *
 * Registers:
 *   t0 - accumulated sum
 *   t1 - next odd number to add
 *   t2 - 0xFFFFFFFF (largest 32-bit unsigned value)
 *   t3 - last odd number actually added
 *   t4 - remaining space (t2 - t0)
 *   t5 - address used to store the result
 */

#include "regdef.h"
#include "cpu.h"
#include "asm.h"

    .text

FRAME(bsp_start, sp, 0, ra)
    .set    noreorder
    move    s0, ra                          /* save return address */

    /* ---- user code ---- */
    .set    reorder
    li      t0, 0                           /* sum = 0 */
    li      t1, 1                           /* first odd number */
    li      t2, -1                          /* 0xFFFFFFFF */
    li      t3, 0                           /* last added number */
cal_loop:
    subu    t4, t2, t0                      /* remaining space */
    bltu    t4, t1, save_result             /* next number would overflow */
    addu    t0, t0, t1                      /* sum += t1 */
    move    t3, t1                          /* remember last added number */
    addu    t1, t1, 2                       /* next odd number */
    b       cal_loop
save_result:
    lui     t5, 0x8003                      /* t5 = 0x80030000 */
    sw      t0, 0(t5)                       /* store the result */
    .set    noreorder
    /* ---- end of user code ---- */

    move    ra, s0                          /* restore return address */
    j       ra
    nop
    .set    reorder
ENDFRAME(bsp_start)
