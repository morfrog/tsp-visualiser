/***************************************************************************/
/*                                                                         */
/*                                 DRAWBOX.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawBox().
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
  z2DPoint p1, p2;

  zRes640();
  p1.x = 50;
  p1.y = 50;
  p2.x = 300;
  p2.y = 300;
  zDrawBox(zSET, 15, &p1, &p2);
  getch();
  zResText();
}
