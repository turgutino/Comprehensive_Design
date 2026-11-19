/*
 * bsp_start.S
 *
 * created: 2021-08-14
 *  author: 
 */

#include "regdef.h"
#include "cpu.h"
#include "asm.h"

//-----------------------------------------------------------------------------

    .text

FRAME(bsp_start,sp,0,ra)
	.set noreorder

    move    s0, ra      /* 返回地址 */

    /*
     * 用户代码插在下面

     */
   
   li  a0, 0xa5a55a5a
   lui v0, 0x8002
   sw  a0, 0(v0)
   nop

   
   lui v0, 0xa002
   lw a1, 0(v0)
   nop
   
   lui v0, 0x8002
   cache 0b10101 ,0(v0)
   nop
   
   lui v0, 0xa002
   lw v1, 0(v0)
   
   li s0, 0x12345678
   lui v0, 0xa003
   sw s0, 0(v0)
   
   lui v0, 0x8003
   lw a2, 0(v0)

    move    ra, s0
    j       ra
	nop
	
	.set reorder
ENDFRAME(bsp_start)

//-----------------------------------------------------------------------------

/*
 * @@ END
 */
 
