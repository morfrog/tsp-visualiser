/***************************************************************************/
/*                                                                         */
/*                               FONTINFO.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zFontGetInfo().
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
  zlong w, h;
  z2DPoint p = {320, 240};
  zchar text[20];

  zRes640();
  zFontGetInfo(&w, &h);
  sprintf((char*)text, "w=%d, h=%d", w, h);
  zDrawString(zSET, 7, 0, text, &p);
  getch();
  zResText();
}
