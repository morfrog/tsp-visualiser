/***************************************************************************/
/*                                                                         */
/*				  SPRITECL.C				    */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zSpriteCollDetect().
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
  zRasterBlock *sprite1, *sprite2;
  z2DPoint p1, p2, p3, p4;
  zlong i, retval;
  zbyte color;
  zchar text[70];

  zRes320();
  sprite1 = (zRasterBlock*)malloc(8 + 30*30);
  sprite2 = (zRasterBlock*)malloc(8 + 50*50);
  if ((sprite1) && (sprite2))
  {
    p1.x = p3.x = 0;
    p1.y = p4.y = 0;
    p2.x = p4.x = 29;
    p2.y = p3.y = 29;
    zDrawFillBox(zSET, 8, &p1, &p2);
    zDrawLine(zSET, 10, &p1, &p2);
    zDrawLine(zSET, 10, &p3, &p4);
    zBlockGet(&p1, &p2, sprite1);
    p1.x = 75;
    p1.y = 75;
    p2.x = 124;
    p2.y = 124;
    p3.x = 100;
    p3.y = 100;
    zDrawFillBox(zSET, 8, &p1, &p2);
    zDrawCircle(zSET, 12, &p3, 24);
    zBlockGet(&p1, &p2, sprite2);
    p1.x = 0;
    p1.y = 0;
    p2.x = 75;
    p2.y = 75;
    p3.x = 0;
    p3.y = 180;
    zBlockPut(zXOR, &p1 ,sprite1);
    for(i=0;i<150;i++)
    {
     p1.x = i;
     p1.y = i;
     zBlockPut(zXOR, &p1 ,sprite1);
     retval = zSpriteCollDetect(8, &p1, &p2, sprite1, sprite2);
     sprintf(text,"the return value is: %d",retval);
     zDrawString(zSET, 7, 0 , text, &p3);
     getch();
     zBlockPut(zXOR, &p1 ,sprite1);
    }
    free(sprite1);
    free(sprite2);
    getch();
    zResText();
  }
}
