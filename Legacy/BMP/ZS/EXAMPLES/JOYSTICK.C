/***************************************************************************/
/*                                                                         */
/*                                JOYSTICK.C                               */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	  */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zJoystickSetTimer() and zJoystickGetStatus.
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

  zJstkStatus jstk;
  zdword jcount, jpresent;
  z2DPoint jAmin, jAmax, jBmin, jBmax;
  z2DPoint p1, p2, p3, p4, cntrA, cntrB, endA, endB;
  zbyte color;

  //initialize our variables
  jcount = 0x4000;
  memset(&jAmin, jcount, sizeof(jAmin));
  memset(&jAmax, 0, sizeof(jAmax));
  memset(&jBmin, jcount, sizeof(jBmin));
  memset(&jBmax, 0, sizeof(jBmax));
  //do the joystick identification/calibration
  zResText();
  jpresent = zWhichJoystick();
  //are any joysticks avaiable?
  if (!jpresent)
  {
    printf("\nSorry...unable to identify any active joysticks...program terminated\n");
    return;
  }
  zJoystickSetTimer(jcount);
  printf("Please move both joysticks to all of their extremes and then press a key...\n");
  while(!kbhit())
  {
    zJoystickGetStatus(&jstk);
    if (jstk.JstkAX < jAmin.x)
      jAmin.x = jstk.JstkAX;
    if (jstk.JstkAY < jAmin.y)
      jAmin.y = jstk.JstkAY;
    if (jstk.JstkAX > jAmax.x)
      jAmax.x = jstk.JstkAX;
    if (jstk.JstkAY > jAmax.y)
      jAmax.y = jstk.JstkAY;
    if (jstk.JstkBX < jBmin.x)
      jBmin.x = jstk.JstkBX;
    if (jstk.JstkBY < jBmin.y)
      jBmin.y = jstk.JstkBY;
    if (jstk.JstkBX > jBmax.x)
      jBmax.x = jstk.JstkBX;
    if (jstk.JstkBY > jBmax.y)
      jBmax.y = jstk.JstkBY;
  }
  getch();
  // give maximums some breathing room
  jAmax.x += 10;
  jAmax.y += 10;
  jBmax.x += 10;
  jBmax.y += 10;
  // determine more efficient count length
  jcount = 0;
  if (jpresent & 1)
  {
    if (jcount < jAmax.x)
      jcount = jAmax.x;
    if (jcount < jAmax.y)
      jcount = jAmax.y;
  }
  if (jpresent & 2)
  {
    if (jcount < jBmax.x)
      jcount = jBmax.x;
    if (jcount < jBmax.y)
       jcount = jBmax.y;
  }
  zJoystickSetTimer(jcount);
  //do the demo
  zRes640();
  cntrA.x = endA.x = 120;
  cntrA.y = endA.y = 120;
  cntrB.x = endB.x = 519;
  cntrB.y = endB.y = 120;
  jAmax.x -= jAmin.x;
  jAmax.x >>= 1;
  jAmin.x += jAmax.x;
  jAmax.y -= jAmin.y;
  jAmax.y >>= 1;
  jAmin.y += jAmax.y;
  jBmax.x -= jBmin.x;
  jBmax.x >>= 1;
  jBmin.x += jBmax.x;
  jBmax.y -= jBmin.y;
  jBmax.y >>= 1;
  jBmin.y += jBmax.y;
  // display for joystick 1
  if (jpresent & 1)
    color = 15;
  else
    color = 8;
  p1.x = 20;
  p1.y = 20;
  p2.x = 221;
  p2.y = 221;
  zDrawBox(zSET, color, &p1, &p2);
  p1.x = 20;
  p1.y = 240;
  p2.x = 221;
  p2.y = 260;
  p3.x = 88;
  p3.y = 244;
  zDrawBox(zSET, color, &p1, &p2);
  p1.x = 20;
  p1.y = 280;
  p2.x = 221;
  p2.y = 300;
  p3.x = 88;
  p3.y = 284;
  zDrawBox(zSET, color, &p1, &p2);
  // display for joystick 2
  if (jpresent & 2)
    color = 15;
  else
    color = 8;
  p1.x = 420;
  p1.y = 20;
  p2.x = 620;
  p2.y = 221;
  zDrawBox(zSET, color, &p1, &p2);
  p1.x = 419;
  p1.y = 240;
  p2.x = 620;
  p2.y = 260;
  p3.x = 487;
  p3.y = 244;
  zDrawBox(zSET, color, &p1, &p2);
  p1.x = 419;
  p1.y = 280;
  p2.x = 620;
  p2.y = 300;
  p3.x = 487;
  p3.y = 284;
  zDrawBox(zSET, color, &p1, &p2);
  // preset button text locations
  p1.x = 88;
  p1.y = 244;
  p2.x = 88;
  p2.y = 284;
  p3.x = 487;
  p3.y = 244;
  p4.x = 487;
  p4.y = 284;
  while(!kbhit())
  {
    zJoystickGetStatus(&jstk);
    if (jpresent & 1)
    {
      zDrawLine(zSET, 0, &cntrA, &endA);
      endA.x = ((jstk.JstkAX - jAmin.x) * 100 / jAmax.x) + cntrA.x;
      endA.y = ((jstk.JstkAY - jAmin.y) * 100 / jAmax.y) + cntrA.y;
      zDrawLine(zSET, 10, &cntrA, &endA);
      color = 8;
      if (jstk.JstkAButs & 1)
        color = 10;
      zDrawString(zSET, color, 0, "Button 1", &p1);
      color = 8;
      if (jstk.JstkAButs & 2)
        color = 10;
      zDrawString(zSET, color, 0, (const zchar*)"Button 2", &p2);
    }
    if (jpresent & 2)
    {
      zDrawLine(zSET, 0, &cntrB, &endB);
      endB.x = ((jstk.JstkBX - jBmin.x) * 100 / jBmax.x) + cntrB.x;
      endB.y = ((jstk.JstkBY - jBmin.y) * 100 / jBmax.y) + cntrB.y;
      zDrawLine(zSET, 10, &cntrB, &endB);
      color = 8;
      if (jstk.JstkBButs & 1)
        color = 10;
      zDrawString(zSET, color, 0, (const zchar*)"Button 1", &p3);
      color = 8;
      if (jstk.JstkBButs & 2)
        color = 10;
      zDrawString(zSET, color, 0, (const zchar*)"Button 2", &p4);
    }
  }
  getch();
  zResText();
}
