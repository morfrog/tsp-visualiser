/***************************************************************************/
/*                                                                         */
/*				 PCXMAKE.C				   */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zPCXMake().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdlib.h>
#include "zsvga.h"

#define PCX_FILE "test.pcx"
#define TEXT_STRING "Made using Zephyr Software's zPCXMake() function!"

void Example(void)
{
  z2DPoint p1, p2;
  zdword i;

  zRes640();
  for(i=0;i<500;i++)
  {
    p1.x = rand() % 640;
    p1.y = rand() % 480;
    p2.x = rand() % 640;
    p2.y = rand() % 480;
    zDrawLine(zSET, rand() % 15 + 1, &p1, &p2);
  }
  p1.x = 0;
  p1.y = 0;
  p2.x = 639;
  p2.y = 479;
  zDrawString(zSET, 15, 0, TEXT_STRING, &p1);
  zPCXMake(&p1, &p2, PCX_FILE);
  zResText();
}
