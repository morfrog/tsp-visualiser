/***************************************************************************/
/*                                                                         */
/*                                SPRITE.C                                 */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zSpriteGAP(), zSpriteGet() and zSpritePut().
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
  zRasterBlock *spriteblk, *spriteback;
  z2DPoint p1, p2;
  zlong i;
  zbyte color;

  zRes640();
  spriteblk = (zRasterBlock*)malloc(8 + 101*101);
  spriteback = (zRasterBlock*)malloc(8 + 101*101);
  if ((spriteblk) && (spriteback))
  {
    p1.x = 50;
    p1.y = 50;
    p2.x = 18;
    p2.y = 44;
    zDrawFillCircle(zSET, 10, &p1, 20);
    zDrawString(zSET, 14, 0, (const zchar*)"!Sprite!", &p2);
    p1.x = 0;
    p1.y = 0;
    p2.x = 100;
    p2.y = 100;
    zDrawBox(zSET, 12, &p1, &p2);
    zBlockGet(&p1, &p2, spriteblk);
    color = 16;
    p1.y = 0;
    p2.y = 479;
    for(i=0;i<639;i++)
    {
      p1.x = i;
      p2.x = i;
      zDrawLine(zSET, color, &p1, &p2);
      if (color++ > 30)
	color =16;
    }
    getch();
    for(i=0;i<50;i++)
    {
      p1.x = i * 3;
      p1.y = i * 3;
      zSpriteGAP(0, &p1, spriteblk, spriteback);
      zSDelay(3);
      zSpritePut(0, &p1, spriteback);
    }
    for(i=50;i>0;i--)
    {
      p1.x = i * 3;
      p1.y = i * 3;
      zSpriteGet(0, &p1, spriteblk, spriteback);
      zSpritePut(0, &p1, spriteblk);
      zSDelay(3);
      zSpritePut(0, &p1, spriteback);
    }
    free(spriteblk);
    free(spriteback);
    getch();
    zResText();
  }
}
