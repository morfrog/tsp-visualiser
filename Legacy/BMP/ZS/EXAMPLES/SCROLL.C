/***************************************************************************/
/*                                                                         */
/*                                SCROLL.C                                 */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zScrollDn(), zScrollLt(), zScrollRt()& zScrollUp().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdlib.h>
#include "zsvga.h"

#define LEFT 220
#define TOP 200
#define WIDTH 160
#define HEIGHT 120

void Example(void)
{
  z2DPoint p1, p2;
  zlong i;
  zRasterBlock* blk;

  zRes640();
  for(i=0;i<500;i++)
  {
    p1.x = rand() % WIDTH + LEFT;
    p1.y = rand() % HEIGHT + TOP;
    zDrawPoint(zSET, rand() % 15 + 1, &p1);
  }
  p1.x = LEFT - 1;
  p1.y = TOP - 1;
  p2.x = LEFT + WIDTH + 1;
  p2.y = TOP + HEIGHT + 1;
  zDrawBox(zSET, 15, &p1, &p2);
  getch();
  p1.x++;
  p1.y++;
  p2.x--;
  p2.y--;
  blk = (zRasterBlock*)malloc(8 + (HEIGHT+1)*(WIDTH+1));
  zBlockGet(&p1, &p2, blk);
  for(i=0;i<HEIGHT;i++)
    zScrollDn(&p1, &p2, 1, 0);
  zBlockPut(zSET, &p1, blk);
  for(i=0;i<HEIGHT;i++)
    zScrollUp(&p1, &p2, 1, 0);
  zBlockPut(zSET, &p1, blk);
  for(i=0;i<WIDTH;i++)
    zScrollLt(&p1, &p2, 1, 0);
  zBlockPut(zSET, &p1, blk);
  for(i=0;i<WIDTH;i++)
    zScrollRt(&p1, &p2, 1, 0);
  getch();
  zResText();
}
