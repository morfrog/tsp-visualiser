/***************************************************************************/
/*                                                                         */
/*                                2DTRANS.C                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of z2DTranslate().
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
  z2DPoint pi[2]={{0,100},{0,200}}, pf[2], offset;

  pf[0] = pi[0];
  pf[1] = pi[1];
  zRes640();
  for(offset.x=0,offset.y=0;offset.x<640;offset.x+=3)
  {
    zDrawLine(zSET, 0, &pf[0], &pf[1]);
    z2DTranslate(2, &offset, pi, pf);
    zDrawLine(zSET, 15, &pf[0], &pf[1]);
    zSDelay(2);
  }
  getch();
  zResText();
}
