/***************************************************************************/
/*                                                                         */
/*                               PALCURTN.H                                */
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


#ifndef PALCURTN_H
#define PALCURTN_H


#include <time.h>


#include "animobj.h"


/*=========================================================================*\
| PaletteCurtainObject
|
| An opening and closing curtain with a time limit.
\*=========================================================================*/
class PaletteCurtainObject : public AnimationObject
{
    private:

		byte _Opening;
		long _LeftIncrement;
		long _RightIncrement;
		z2DPoint _LeftP;
		z2DPoint _RightP;
		z2DPoint _TLView, _BRView;
		byte _Color;
		byte _BackFillColor;
		zPalette _Palette;
        time_t _Timer;
        char _TimerPrompt[2];
        z2DPoint _TimerPosition;

    public:

		PaletteCurtainObject(void);

		~PaletteCurtainObject(void) {}

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // PALCURTN_H
