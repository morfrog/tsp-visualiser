/***************************************************************************/
/*                                                                         */
/*                                DRAWCBEZ.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawCubicBezier().
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
  z2DPoint p1 = { 50,	50};
  z2DPoint p2 = {100, 150};
  z2DPoint p3 = {150,	50};
  z2DPoint p4 = {200, 150};

  zRes640();
  zDrawCubicBezier(zSET, 15, &p1, &p2, &p3, &p4);
  getch();
  zResText();
}
