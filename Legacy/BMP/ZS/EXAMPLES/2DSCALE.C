/***************************************************************************/
/*                                                                         */
/*                                2DSCALE.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of z2DScale().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdlib.h>
#include "zsvga.h"

void Example(void)
{
  zangle s;
  z2DPoint p1={0,0}, p2i={320,240}, p2f;

  p2f = p2i;
  zRes640();
  for(s=0x4000;s<0x20000;s+=0x200)
  {
    zDrawBox(zSET, 0, &p1, &p2f);
    z2DScale(1, s, s, &p2i, &p2f);
    zDrawBox(zSET, 15, &p1, &p2f);
    zSDelay(2);
  }
  getch();
  zResText();
}
