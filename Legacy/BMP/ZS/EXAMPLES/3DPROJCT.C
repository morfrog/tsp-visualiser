/***************************************************************************/
/*                                                                         */
/*                                3DPROJCT.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of z3DProject().
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
  z3DPoint viewoffset = {0, 0, 1000};
  z3DPoint origin = {0, 0, 0};
  z3DPoint tetra[4] = {   {   0,  200, -320}, {   0, -320,    0},
			  {-300,  200,	200}, { 300,  200,  200} };
  z2DPoint plot[4];
  zdword i;

  zRes640();
  z3DTranslate(4, &viewoffset, tetra, tetra);
  z3DProject(4, 600, tetra, plot);
  for(i=1;i<4;i++)
    zDrawLine(zSET, 15, plot, plot+i);
  for(i=2;i<4;i++)
    zDrawLine(zSET, 15, plot+1, plot+i);
  zDrawLine(zSET, 15, plot+2, plot+3);
  getch();
  zResText();
}
