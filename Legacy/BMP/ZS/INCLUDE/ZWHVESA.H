/***************************************************************************/
/*                                                                         */
/*								  ZWHVESA.H							       */
/*                                                                         */
/***************************************************************************/
/*		(c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains the zWhichVESA() prototype, a function that provides
| information necessary for VESA support in the ZSVGA graphics library.  This
| involves allocating and freeing a DOS real mode segment.
|
| See the source for the ZSVGA demo program for sample usage.
|
| Revision History:
|
| Sep 26, 1995: First pass complete
\--------------------------------------------------------------------------*/


#ifndef ZWHVESA_H
#define ZWHVESA_H


#include "ztypes.h"


/*---------------------------------------------------------------------------
| Define the minimum size of the information block
*/
#define zVESAINFOSIZE 1536


/*---------------------------------------------------------------------------
| Register block used by the 0x300 DPMI call (simulate real mode interrupt)
| in DOS/4GW.
*/
#ifdef __WATCOMC__
typedef struct tag_RealModeREGS
{
	long edi;
	long esi;
	long ebp;
	long reserved;
	long ebx;
	long edx;
	long ecx;
	long eax;
	short flags;
	short es, ds, fs, gs, ip, cs, sp, ss;
} RealModeREGS;
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------
| zWhichVESA()
|
| Fills the VESAInfo block with VESA specific information necessary for VESA
| support in the ZSVGA graphics library.  The VESAInfo block is divided into
| six 256 byte chunks.	The first 256 bytes contain information concerning
| general VESA support for the card.  The remaining 5 chunks give information
| specific to the five resolutions supported: 640x400, 640x480, 800x600,
| 1024x768, and 1280x1024.
|
| This function requires allocating and freeing a DOS real mode segment.
|
| Inputs: VESAInfo     - pointer to a zVESAINFOSIZE byte block to receive the
|                        VESA configuration information
|         PhysicalZero - pointer to physical address zero
|
| Return: bit field indicating modes supported, zero if unsuccessful
*/
zbyte zWhichVESA(void *VESAInfo, void *PhysicalZero);


#ifdef __cplusplus
}
#endif


/*=========================================================================*/
#endif /* ZWHVESA_H */
