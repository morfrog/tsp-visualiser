/***************************************************************************/
/*                                                                         */
/*                               PAGEACTV.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zPageActive() and zPageDisplay().
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
  z2DPoint pgoff = {0, 0};
  zchar text1[] = "Page 0";
  zchar text2[] = "Page 1";
  zchar texterr[] = "Paging not supported";

  zRes640();
  if(zPageDisplay(0, &pgoff, zFALSE))
  {
    zDrawString(zSET, 0, 7, text1, &p);
    zPageActive(1);
    zDrawString(zSET, 0, 7, text2, &p);
    getch();
    zPageDisplay(1, &pgoff, zTRUE);
  }
  else
  {
    zDrawString(zSET, 0, 7, texterr, &p);
  }
  getch();
  zResText();
}
