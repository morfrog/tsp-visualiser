/***************************************************************************/
/*                                                                         */
/*                                  PALIOA.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zPalIOAuto().
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
  z2DPoint line[2] = {{0, 0}, {0, 480}};
  zPalette pal;
  zlong i;

  for(i=0;i<64;i++)
  {
    pal.Color[i].r = i; pal.Color[i].g = 0; pal.Color[i].b = 0;
    pal.Color[i+64].r = 0; pal.Color[i+64].g = i; pal.Color[i+64].b = 0;
    pal.Color[i+128].r = 0; pal.Color[i+128].g = 0; pal.Color[i+128].b = i;
    pal.Color[i+192].r = i; pal.Color[i+192].g = i; pal.Color[i+192].b = i;
  }
  zRes640();
  zPalSet(&pal, 0, 255, zTRUE);
  for(i=0;i<zMaxX;i++)
  {
    zDrawLine(zSET, (zbyte)(i & 0xFF), line, line+1);
    line[0].x++; line[1].x++;
  }
  getch();
  zPalIOAuto(&pal, 0, 255, -64);
  zPalIOAuto(&pal, 64, 191, 64);
  getch();
  zResText();
}
