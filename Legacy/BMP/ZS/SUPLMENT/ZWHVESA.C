/***************************************************************************/
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/*		(c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains the zWhichVESA() function that provides information
| necessary for VESA support in the ZSVGA graphics library.  This involves
| allocating and freeing a DOS real mode segment of zVESAINFOSIZE bytes.
|
| See the source for the ZSVGA demo program for sample usage.
|
| Revision History:
|
| Sep 26, 1995: First pass complete
| Sep 27, 1995: Add check for VESA signature
\--------------------------------------------------------------------------*/


#ifdef __SC__
#include <dos.h>
#endif
#ifdef __WATCOMC__
#include <i86.h>
#endif
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zwhvesa.h"


/*---------------------------------------------------------------------------
| Static (hidden) Function prototypes.  Definitions found at end of file.
*/
static zword _AllocateDOSSeg(zdword NumBytes, zword* Segment);
static void _FreeDOSSeg(zword Value);
static zbyte _VESAGetInfo(zword Segment, zword Offset);
static zbyte _VESAModeGetInfo(zword Mode, zword Segment, zword Offset);


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
zbyte zWhichVESA(void *VESAInfo, void *PhysicalZero)
{
	zword selector; // holds the segment in Symantec
	zword seg;
        zbyte *ptr = (zbyte*)PhysicalZero;
	zbyte result = 0;

    if(!VESAInfo)
        return(result);

	memset(VESAInfo, 0, zVESAINFOSIZE);

	selector = _AllocateDOSSeg(zVESAINFOSIZE, &seg);

	if(seg)
	{
		ptr += (seg<<4);
		memset(ptr, 0, zVESAINFOSIZE);

		if(_VESAGetInfo(seg, 0))
		{
			// 0x100: 640x400x256
			if(_VESAModeGetInfo(0x100, seg, 256))
				result |= 1;

			// 0x101: 640x480x256
			if(_VESAModeGetInfo(0x101, seg, 512))
				result |= 2;

			// 0x103: 800x600x256
			if(_VESAModeGetInfo(0x103, seg, 768))
				result |= 4;

			// 0x105: 1024x768x256
			if(_VESAModeGetInfo(0x105, seg, 1024))
				result |= 8;

			// 0x107: 1280x1024x256
			if(_VESAModeGetInfo(0x107, seg, 1280))
				result |= 0x10;
		}

		memcpy(VESAInfo, ptr, zVESAINFOSIZE);

		_FreeDOSSeg(selector);
	}

    // check for 'VESA' signature
    if(!result || (*(zdword*)ptr != 0x41534556))
        return(0);

	return(result);
}


/*---------------------------------------------------------------------------
| _AllocateDOSSeg()
|
| Allocates a DOS real mode segment of the specified size.  Implementation of
| this function is compiler dependent.
|
| Inputs: NumBytes - size of segment
|         Segment  - pointer to variable to receive allocated segment
|
| Return: zero if unsuccessful, compiler dependent if successful:
|         Symantec: returns the allocated segment
|         Watcom: returns a selector for the allocated segment
*/
static zword _AllocateDOSSeg(zdword NumBytes, zword* Segment)
{
	union REGS regs;

	// round up if necessary and convert to paragraphs
	if(NumBytes & 0x0F)
		NumBytes += 0x10;
	NumBytes >>= 4;

	*Segment = NULL;

	#ifdef __SC__
		regs.h.ah = 0x48;
		regs.x.bx = (zword)NumBytes;
		int86_real(0x21, &regs, &regs);
		if(regs.x.cflag == 0)
		{
			*Segment = regs.x.ax;
			return(regs.x.ax);
		}
	#endif

	#ifdef __WATCOMC__
		regs.w.ax = 0x100;
		regs.w.bx = (zword)NumBytes;
		int386(0x31, &regs, &regs);
		if(regs.x.cflag == 0)
		{
			*Segment = regs.w.ax;
			return(regs.w.dx);
		}
	#endif

	return(0);
}


/*---------------------------------------------------------------------------
| _FreeDOSSeg()
|
| Frees a DOS real mode segment.  Implementation of this function is compiler
| dependent.
|
| Inputs: Value - compiler dependent:
|         Symantec: segment to be deallocated
|         Watcom: selector of segment to be deallocated
|
| Return: none
*/
static void _FreeDOSSeg(zword Value)
{
	union REGS regs;

	#ifdef __SC__
		struct SREGS sregs;
		regs.h.ah = 0x49;
		sregs.es = Value;
		int86x_real(0x21, &regs, &regs, &sregs);
	#endif

	#ifdef __WATCOMC__
		regs.w.ax = 0x101;
		regs.w.dx = Value;
		int386(0x31, &regs, &regs);
	#endif
}


/*---------------------------------------------------------------------------
| _VESAGetInfo
|
| Retrieves the general VESA information from the BIOS of the video card and
| places it at the specified offset of the given segment.  The segment must
| be a DOS real mode segment.  This information is 256 bytes in length.
|
| Inputs: Segment - a DOS real mode segment
|         Offset  - offset into the segment
|
| Return: one if successful, zero otherwise
*/
static zbyte _VESAGetInfo(zword Segment, zword Offset)
{
	union REGS regs;
	struct SREGS sregs;

	#ifdef __SC__
		regs.x.ax = 0x4F00;
		regs.x.di = Offset;
		sregs.es = Segment;
		int86x_real(0x10, &regs, &regs, &sregs);
		if(regs.x.ax == 0x004F)
			return(1);
	#endif

	#ifdef __WATCOMC__
		RealModeREGS rmregs;
		memset(&rmregs, 0, sizeof(rmregs));
		memset(&sregs, 0, sizeof(sregs));
		rmregs.eax = 0x4F00;
		rmregs.edi = Offset;
		rmregs.ds = Segment;
		rmregs.es = Segment;
		regs.w.ax = 0x300;
		regs.h.bl = 0x10;
		regs.h.bh = 0;
		regs.w.cx = 0;
		sregs.es = FP_SEG(&rmregs);
		regs.x.edi = FP_OFF(&rmregs);
		int386x(0x31, &regs, &regs, &sregs);
		if(rmregs.eax == 0x004F)
			return(1);
	#endif

	return(0);
}


/*---------------------------------------------------------------------------
| _VESAModeGetInfo
|
| Retrieves the VESA information for a specific mode from the BIOS of the
| video card and places it at the specified offset of the given segment.  The
| segment must be a DOS real mode segment.  This information retrieved is 256
| bytes in length.
|
| Inputs: Mode    - desired VESA mode number
|         Segment - a DOS real mode segment
|         Offset  - offset into the segment
|
| Return: one if successful, zero otherwise
*/
static zbyte _VESAModeGetInfo(zword Mode, zword Segment, zword Offset)
{
	union REGS regs;
	struct SREGS sregs;

	#ifdef __SC__
		regs.x.ax = 0x4F01;
		regs.x.cx = Mode;
		regs.x.di = Offset;
		sregs.es = Segment;
		int86x_real(0x10, &regs, &regs, &sregs);
		if(regs.x.ax == 0x004F)
			return(1);
	#endif

	#ifdef __WATCOMC__
		RealModeREGS rmregs;
		memset(&rmregs, 0, sizeof(rmregs));
		memset(&sregs, 0, sizeof(sregs));
		rmregs.eax = 0x4F01;
		rmregs.ecx = Mode;
		rmregs.edi = Offset;
		rmregs.es = Segment;
		rmregs.ds = Segment;
		regs.w.ax = 0x300;
		regs.h.bl = 0x10;
		regs.h.bh = 0;
		regs.w.cx = 0;
		sregs.es = FP_SEG(&rmregs);
		regs.x.edi = FP_OFF(&rmregs);
		int386x(0x31, &regs, &regs, &sregs);
		if(rmregs.eax == 0x004F)
			return(1);
	#endif

	return(0);
}
