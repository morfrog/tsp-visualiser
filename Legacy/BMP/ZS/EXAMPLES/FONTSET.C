/***************************************************************************/
/*                                                                         */
/*                                FONTSET.C                                */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zFontSet().
|
| Revision History:
|
| Dec 11, 1995: First pass complete
\--------------------------------------------------------------------------*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "zsvga.h"

void Example(void)
{
  z2DPoint p = {320, 240};
  zchar text1[] = "Dragon Font";
  zchar text2[] = "System Font";
  zFont font;
  FILE *fp;

  zRes640();
  fp=fopen("dragon.fnt","rb");
  fread(&font, 1, sizeof(font), fp);
  fclose(fp);
  zFontSet(&font);
  zDrawString(zSET, 7, 0, text1, &p);
  zFontSystem();
  p.y += 20;
  zDrawString(zSET, 7, 0, text2, &p);
  getch();
  zResText();
}
