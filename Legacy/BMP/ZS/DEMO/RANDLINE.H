/***************************************************************************/
/*                                                                         */
/*                               RANDLINE.H                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This is a demonstration of the capabilities of the ZSVGA graphics library.
|
| Revision History:
|
| Jan 17, 1996: First pass complete.
\--------------------------------------------------------------------------*/


#ifndef RANDLINE_H
#define RANDLINE_H


#include "animobj.h"


class RandomLinesObject : public AnimationObject
{
	private:

		z2DPoint _P0, _P1;
		byte _Color;

	public:

		RandomLinesObject(void);

		~RandomLinesObject(void) {}

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // RANDLINE_H
