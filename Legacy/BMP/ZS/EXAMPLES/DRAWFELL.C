/***************************************************************************/
/*                                                                         */
/*                                DRAWFELL.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawFillEllipse().
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
  z2DPoint p = {320, 240};
  z2DPoint r = { 75, 150};

  zRes640();
  zDrawFillEllipse(zSET, 15, &p, &r);
  getch();
  zResText();
}
