/***************************************************************************/
/*                                                                         */
/*                               GETPOINT.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zGetPoint().
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
  z2DPoint p = {50, 50};
  zbyte c1, c2;

  zRes640();
  zDrawPoint(zSET, 15, &p);
  c1 = zGetPoint(&p);
  p.x++;
  c2 = zGetPoint(&p);
  getch();
  zResText();
  printf("point 1 was color %d, point 2 was color %d\n",c1, c2);
}
