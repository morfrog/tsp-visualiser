/***************************************************************************/
/*                                                                         */
/*                               BLOCKGET.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zBlockGet() and zBlockPut().
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
  zRasterBlock *blk;
  z2DPoint p1, p2;

  zRes640();
  p1.x = 100;
  p1.y = 100;
  zDrawFillCircle(zSET, 15, &p1, 50);
  blk = (zRasterBlock*)malloc(8 + 101*101);
  if (blk)
  {
    p1.x = 50;
    p1.y = 50;
    p2.x = 150;
    p2.y = 150;
    zBlockGet(&p1, &p2, blk);
    getch();
    p1.x = 300;
    p1.y = 50;
    zBlockPut(zSET, &p1, blk);
    free(blk);
  }
  getch();
  zResText();
}
