/***************************************************************************/
/*                                                                         */
/*                               2DROTATE.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of z2DRotate().
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
  zangle a;
  z2DPoint p1={320,240}, p2i={220,240}, p2f;

  p2f = p2i;
  zRes640();
  for(a=0;a<=0x10000;a+=0x100)
  {
    zDrawLine(zSET, 0, &p1, &p2f);
    z2DRotate(1, &p1, a, &p2i, &p2f);
    zDrawLine(zSET, 15, &p1, &p2f);
    zSDelay(2);
  }
  getch();
  zResText();
}
