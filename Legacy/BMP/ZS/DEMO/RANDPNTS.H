/***************************************************************************/
/*                                                                         */
/*                               RANDPNTS.H                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
|
|
| Revision History:
|
| Aug 19, 1995: First pass complete.
| Dec 03, 1995: File separation
\--------------------------------------------------------------------------*/


#ifndef RANDPNTS_H
#define RANDPNTS_H


#include "animobj.h"


class RandomPointsObject : public AnimationObject
{
	private:

		z2DPoint _P0;
		byte _Color;

	public:

		RandomPointsObject(void);

		~RandomPointsObject(void) {}

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // RANDPNTS_H
