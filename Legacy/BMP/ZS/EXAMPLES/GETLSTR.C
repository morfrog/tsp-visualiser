/***************************************************************************/
/*                                                                         */
/*                                GETLSTR.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zGetLastString().
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
  zchar text[] = "Hello, World";
  z2DPoint box[2] = {{200, 200}, {400, 400}};
  zRasterBlock *tmp;
  zRasterBlock *blk;
  zRasterBlock *back;

  back = (zRasterBlock*)malloc(8*sizeof(text)*14 + 8);
  if(!back)
    return;

  blk = (zRasterBlock*)malloc(8*sizeof(text)*14 + 8);
  if(blk)
  {
    zRes640();
    zDrawFillBox(zSET, 15, box, box+1);
    zDrawString(zNO_ACTION, 255, 7, text, box);
    getch();
    tmp = zGetLastString();
    zByteCopy(tmp, blk, 8*sizeof(text)*14 + 8);
    box[0].x = 275;
    for(box[0].y=100;box[0].y<480;box[0].y++)
    {
      zSpriteGAP(255, box, blk, back);
      zSDelay(1);
      zSpritePut(255, box, back);
    }
    getch();
    zResText();
  }
}
