/***************************************************************************/
/*                                                                         */
/*                                SQUARES.H                                */
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


#ifndef SQUARES_H
#define SQUARES_H


#include "animobj.h"


class SquaresObject : public AnimationObject
{
	private:

#define POINTS_PER_SQUARE 4
		z2DPoint _WorkingSquare[POINTS_PER_SQUARE];

#define NUM_SQUARES 9
		z2DPoint _PlotPoints[NUM_SQUARES * POINTS_PER_SQUARE];
#define NUM_PLOT_POINTS (sizeof(_PlotPoints)/sizeof(_PlotPoints[0]))
		z2DPoint _PlotView[2];

		zangle _AngleOnCenter[NUM_SQUARES];
#define NUM_SQUARES_ROT_ON_CENTER NUM_SQUARES

		zangle _AngleInOrbit[NUM_SQUARES - 1];
#define NUM_SQUARES_IN_ORBIT (NUM_SQUARES - 1)

		byte _SquareColor[NUM_SQUARES];

	public:

        SquaresObject(void);

        ~SquaresObject(void) {}

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // SQUARES_H
