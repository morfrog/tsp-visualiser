/***************************************************************************/
/*                                                                         */
/*                               FILLAREA.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zFillArea().
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
  z2DPoint box[2] = {{50, 50}, {250, 250}};
  z2DPoint circle = {200, 200};
  z2DPoint seed = {100, 100};

  zRes640();
  zDrawBox(zSET, 15, box, box+1);
  zDrawCircle(zSET, 12, &circle, 30);
  zDrawCircle(zSET, 15, box+1, 30);
  getch();
  zFillArea(&seed, 15, 7);
  getch();
  zResText();
}
