/***************************************************************************/
/*                                                                         */
/*				 PCXPUT.C				   */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zPCXGetInfo() & zPCXPut().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdlib.h>
#include "zsvga.h"

#define PCX_FILE "zephyr.pcx"

void Example(void)
{
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
  getch();
  zResText();
}
