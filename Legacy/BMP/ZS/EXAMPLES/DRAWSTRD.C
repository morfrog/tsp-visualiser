/***************************************************************************/
/*                                                                         */
/*                                DRAWSTRD.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zDrawStringDn().
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

  zRes640();
  zDrawStringDn(zSET, 0, 7, "Using zDrawStringDn()", &p);
  getch();
  zResText();
}
