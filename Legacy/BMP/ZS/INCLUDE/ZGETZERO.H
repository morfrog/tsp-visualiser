/***************************************************************************/
/*                                                                         */
/*                               ZGETZERO.H                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains prototypes for the sample function implementations for
| retrieving a near pointer to the lower megabyte of physical memory for
| three major 32 bit DOS compiler/extender combinations.
|
| Revision History:
|
| Sep 26, 1995: First pass complete
\--------------------------------------------------------------------------*/


#ifndef ZGETZERO_H
#define ZGETZERO_H


#include "ztypes.h"


#ifdef __cplusplus
extern "C" {
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
void* zGetPhysicalZero(void);


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
void zDisablePhysicalZero(void);


#ifdef __cplusplus
}
#endif


/*=========================================================================*/
#endif /* ZGETZERO_H */
