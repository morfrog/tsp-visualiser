/***************************************************************************/
/*                                                                         */
/*                               ZGETZERO.C                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains sample function implementations to retrieve a near 
| pointer to the lower megabyte of physical memory for three major 32 bit
| DOS compiler/extender combinations.  There are basically two routines:
| zGetPhysicalZero() and zDisablePhysicalZero().  zGetPhysicalZero() makes
| the appropriate calculations and returns a pointer to the lower megabyte of
| physical memory.  It should be called early in the application so the
| returned pointer can be passed to zInitZSVGA().  zDisablePhysicalZero()
| completes any cleanup required and should be called just before the
| application exits.  Naturally, no functions in the ZSVGA library should be
| called after zDisablePhysicalZero() has completed.  For two of the three
| compilers zDisablePhysicalZero() does nothing and is merely provided for
| completeness.
|
| Revision History:
|
| Aug 15, 1995: First pass complete
| Sep 26, 1995: Reworked into a .c file
\--------------------------------------------------------------------------*/


#include <dos.h>
#include <io.h>
#include <stdio.h>

#include "ztypes.h"


/*---------------------------------------------------------------------------
| Prototypes for Borland code found at end of this file.
*/
#ifdef __BORLANDC__
void* _CreateMemoryAlias(unsigned long mem, unsigned long len);
void _DeleteMemoryAlias(void);
#endif


/*---------------------------------------------------------------------------
| zGetPhysicalZero()
|
| Calculate a near pointer to physical address zero for accessing the lower
| megabyte.  If one of the following compilers is not in use, return a -1 
| with hopes it will trigger a fault.
|
| Watcom + Rational Systems' DOS4G/W
| All of the selectors a have a base address of zero with a 4 Gigabyte limit.
| A pointer to physical zero is simply a null pointer.
|
| Symantec + Flashtek DOS extender
| The DS selector accepts a negative offset.  Therefore, a pointer to the
| lower megabyte is just the negated base address of DS.
| 
| Borland 4.x + PowerPack
| The only working method originates from a sample file, linear.c, found on 
| the Borland BBS.  It involves mapping the lower megabyte of memory into
| DS's space.  Unfortunately, as pointed out in the comments of linear.c,
| this does not work under any version of Windows.  As Borland's own graphics
| library, BGI32, has exactly the same limitation, this is likely the only
| correct method.
|
| Inputs: none
|
| Return: near pointer to the beginning of physical memory.
*/
void* zGetPhysicalZero(void)
{
	#if defined __WATCOMC__

		return(NULL);

	#elif defined __SC__

		zlong x;
		zbyte far* tmpptr;
		tmpptr = MK_FP(getDS(),0);
		x = (zlong)_x386_get_abs_address(tmpptr);
		x = -x;
		return((void*)x);

	#elif defined __BORLANDC__

		return(_CreateMemoryAlias(0x0000, 0x100000));

	#else

		return((void*)0xFFFFFFFF);

	#endif
}

/*---------------------------------------------------------------------------
| zDisablePhysicalZero()
|
| Do any cleanup required from accessing the lower megabyte of physical
| memory.  
|
| Watcom + Rational Systems' DOS4G/W
| No cleanup required.
|
| Symantec + Flashtek DOS extender
| No cleanup required.
| 
| Borland 4.x + PowerPack
| Unmap the region.
|
| Inputs: none
|
| Return: none
*/
void zDisablePhysicalZero(void)
{
	#if defined __WATCOMC__

		return;

	#elif defined __SC__

		return;

	#elif defined __BORLANDC__

		_DeleteMemoryAlias(0x0000, 0x100000);
		return;

	#else

		return;

	#endif
}


/*---------------------------------------------------------------------------
| The following code originated from the sample program linear.c found on
| the Borland BBS.
*/
#ifdef __BORLANDC__

#define _PAGESIZE 0x1000 // page size is always 4k

// Assembly macros so bcc32 doesn't need to call tasm32 (for speed)
#define _LAR_EAX_EBX  {__emit__ (0x0F); __emit__(0x02); __emit__(0xC3);}
#define _LSL_EAX_EBX  {__emit__ (0x0F); __emit__(0x03); __emit__(0xC3);}
#define _PUSH_BX      {__emit__ (0x53);}
#define _POP_BX       {__emit__ (0x5B);}

unsigned long _hMem;   	 // This holds the DPMI handle to the address space
						 // we will allocate.

unsigned long _AllocAndMapMemory(unsigned long ConvMemAddr, unsigned long Pages, unsigned long *hMemBlk)
{
  unsigned long offset;
  unsigned long Len;
  Len = Pages * 0x1000;

  _EBX = 0;              // let DPMI pick the address
  _ECX = Len;            // length in bytes
  _EDX = 0;              // uncommitted
  _AX = 0x0504;          // DPMI 0504h: Allocate Linear Memory Block
  geninterrupt(0x31);    // call DPMI
  if (_FLAGS&1)
  {
    *hMemBlk = 0;
    return 0;
  }
  offset = _EBX;         // DPMI returns the new linear address
  *hMemBlk = _ESI;       // DPMI returns the handle to this memory block

  // _ESI is already set
  _EBX = 0;              // start at offset 0 into our memory block
  _ECX = Pages;          // the number of pages to map
  _EDX = ConvMemAddr;    // the target address range to be mapped in
  _AX = 0x0508;          // DPMI 0508h: Map Device in Memory Block
  geninterrupt(0x31);    // call DPMI
  if (_FLAGS&1)
  {
    *hMemBlk = 0;
    return 0;
  }
  return offset;
}

unsigned long _GetLimit (unsigned short sel)
{
  _EBX = 0;
  _BX = sel;
  _LSL_EAX_EBX;
  return _EAX;
}

void _SetLimit (unsigned short sel, unsigned long off)
{
  unsigned short h,l;
  h = (unsigned short)(off >> 16);
  l = (unsigned short)(off & 0xFFFF);
  _AX = 8;               // DPMI 0008h: Set Selector Limit
  _BX = sel;
  _CX = h;
  _DX = l;
  geninterrupt(0x31);    // call DPMI
}

unsigned long _GetBase (unsigned short s)
{
  unsigned short upr, lwr;

  _PUSH_BX;              // save EBX accross this function
  _EBX = 0;
  _EAX = 0;
  _BX = s;
  _AX = 6;               // DPMI 0006h: Get Selector Base Address
  geninterrupt(0x31);    // call DPMI
  _POP_BX;               // restore EBX
  if (_FLAGS&1)
    return -1;
  upr = _CX;
  lwr = _DX;
  return ((unsigned long)upr)*0x10000L+lwr;
}

void* _CreateMemoryAlias(unsigned long mem, unsigned long len)
{
  unsigned long linear, length;
  length = (len + _PAGESIZE - 1) / _PAGESIZE;
  linear = AllocAndMapMemory(mem, length, &_hMem); // Allocate address space
                                                   // and map in our memory.

  _SetLimit(_DS, _GetLimit(_DS) + (length*_PAGESIZE)); // increase DS's limit
  return((void *)(linear - _GetBase(_DS)));
}

void _DeleteMemoryAlias(void)
{
  _ESI = _hMem;
  _ESI = _ESI >> 16;
  _EDI = _hMem;          // SI:DI gets 32-bit hMem
  _AX = 0x0502;          // DPMI 0502h: Free Memory Block
  geninterrupt(0x31);    // call DPMI
}

#endif /* __BORLANDC__ */
