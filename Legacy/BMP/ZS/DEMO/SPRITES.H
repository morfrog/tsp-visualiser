/***************************************************************************/
/*                                                                         */
/*                                SPRITES.H                                */
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


#ifndef SPRITES_H
#define SPRITES_H


#include "animobj.h"


class SpritesObject : public AnimationObject
{
	private:

		zRasterBlock* _Sprite;
		zRasterBlock* _Background1;
		zRasterBlock* _Background2;

		z2DPoint _Position1;
		z2DPoint _Increment1;

		z2DPoint _Position2;
		z2DPoint _Increment2;

	public:

        SpritesObject(void);

        ~SpritesObject(void);

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // SPRITES_H
