/***************************************************************************/
/*                                                                         */
/*                                 PALCHGS.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zPalChgStep().
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
  zPalette pal1, pal2;
  zdword i;

  for(i=0;i<64;i++)
  {
    pal2.Color[i].r = i; pal2.Color[i].g = 0; pal2.Color[i].b = 0;
    pal2.Color[i+64].r = 0; pal2.Color[i+64].g = i; pal2.Color[i+64].b = 0;
    pal2.Color[i+128].r = 0; pal2.Color[i+128].g = 0; pal2.Color[i+128].b = i;
    pal2.Color[i+192].r = i; pal2.Color[i+192].g = i; pal2.Color[i+192].b = i;
  }
  zRes640();
  zPalGet(&pal1, 0, 255);
  for(i=0;i<zMaxX;i++)
  {
    zDrawLine(zSET, (zbyte)(i & 0xFF), line, line+1);
    line[0].x++; line[1].x++;
  }
  getch();
  for(i=0;i<0x10000;i+=0x4FF)
    zPalChgStep(&pal1, &pal2, 0, 255, (z1616fix)i, zTRUE);
  getch();
  zResText();
}
