/***************************************************************************/
/*                                                                         */
/*                               BLKROTAT.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zBlockRotate().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdlib.h>
#include "zsvga.h"

#define PCX_FILE "zephyr.pcx"
#define ANGLE 0x1000

void Example(void)
{
  zRasterBlock *blk = NULL;
  zRasterBlock *blk2 = NULL;
  zbyte* ptmp = NULL;
  z2DPoint p1, p2;
  zPalette pal;
  zdword a, b;

  zRes640();
  zFillScreen(0);
  zPCXGetInfo(PCX_FILE, (zdword*)&p2.x, (zdword*)&p2.y, &a, &pal);
  zPalSet(&pal, 0, 255, zFALSE);
  p1.x = 0;
  p1.y = 0;
  zPCXPut(zSET, &p1, PCX_FILE);
  blk = (zRasterBlock*)malloc(8 + p2.x * p2.y);
  if(blk)
  {
    p2.x--;
    p2.y--;
    zBlockGet(&p1, &p2, blk);
    zBlockRotateSize(ANGLE, blk, &a, &b);
    blk2 = (zRasterBlock*)malloc(b);
    ptmp = (zbyte*)malloc(a);
  }
  if(blk2 && ptmp)
  {
    zSetTempBuffer(ptmp, a);
    zBlockRotate(ANGLE, 0, blk, blk2);
    zBlockPut(zSET, &p1, blk2);
  }
  free(blk);
  free(blk2);
  free(ptmp);
  getch();
  zResText();
}
