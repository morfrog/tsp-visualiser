/***************************************************************************/
/*                                                                         */
/*                                 MOUSE.C                                 */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of the sample mouse implementation.
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "zmouse.h"
#include "zsvga.h"

void Example(void)
{
  z2DPoint p1;
  unsigned x, y;
  z2DPoint pos;
  char text[20];
  zdword status;

  zRes640();
  for(x=0;x<200;x++)
  {
    p1.x = rand() % 640;
    p1.y = rand() % 480;
    pos.x = rand() % 640;
    pos.y = rand() % 480;
    y = (rand() % 15) + 1;
    zDrawLine(zSET, y, &p1, &pos);
  }
  if(zMouseEnable())
  {
    zMouseShow();
    p1.x = 100;
    p1.y = 100;
    while(!kbhit())
    {
      zMouseGetStatus(&status, &pos);
      sprintf(text,"%4d %4d %08lX",pos.x, pos.y, status);
      zDrawString(zSET,10,1,(const zchar*)text,&p1);
    }
    getch();
    zMouseDisable();
    zResText();
  }
  else
  {
    zResText();
    printf("mouse not found\n");
  }
}
