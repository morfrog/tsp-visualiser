/***************************************************************************/
/*                                                                         */
/*                                 DRAWCARC.C                              */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawCirArc().
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
  z2DPoint p;

  zRes640();
  p.x = 320;
  p.y = 240;
  zDrawCirArc(zSET, 15, &p, 100, 0xC000, 0x14000);
  getch();
  zResText();
}
