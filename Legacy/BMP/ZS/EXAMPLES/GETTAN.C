/***************************************************************************/
/*                                                                         */
/*                                GETTAN.C                                 */
/*                                                                         */
/***************************************************************************/
/*	(c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill	   */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Demonstrate the use of zGetTan().
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
  printf("The tangent of 0x2000 (45 degrees) is %f\n",(float)zGetTan(0x2000)/65536.0);
}
