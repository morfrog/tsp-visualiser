/***************************************************************************/
/*                                                                         */
/*                               FILLCPOL.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zFillConvexPoly().
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
  z2DPoint poly[4] = {{320, 190}, {270, 240}, {320, 290}, {370, 240}};

  zRes640();
  zFillConvexPoly(15, 4, poly);
  getch();
  zResText();
}
