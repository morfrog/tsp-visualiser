/***************************************************************************/
/*                                                                         */
/*                                 DRAWAALN.C                              */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawAALine().
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
  zPalette pal;
  zlong i, c;
  z2DPoint p[2] = {{ 50,  50}, {500, 300}};

  zRes640();
  zPalGet(&pal, 0, 255);
  for(i=63,c=16;c<24;i-=5,c++)
  {
    pal.Color[c].r = i;
    pal.Color[c].g = 0;
    pal.Color[c].b = 0;
  }
  zPalSet(&pal, 0, 255, zTRUE);
  zDrawAALine(3, 16, p, p+1);
  p[0].x += 10; p[1].x += 10;
  zDrawLine(zSET, 16, p, p+1);
  getch();
  zResText();
}
