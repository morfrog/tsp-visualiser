/***************************************************************************/
/*                                                                         */
/*                                 CUBES.H                                 */
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


#ifndef CUBES_H
#define CUBES_H


#include "animobj.h"


class CubesObject : public AnimationObject
{
	private:

#define POINTS_PER_CUBE 8
#define NUM_CUBES 9
		z3DPoint _WorkingCubes[NUM_CUBES * POINTS_PER_CUBE];
		z2DPoint _PlotPoints[NUM_CUBES * POINTS_PER_CUBE];
#define NUM_PLOT_POINTS (sizeof(_PlotPoints)/sizeof(_PlotPoints[0]))
		z2DPoint _PlotView[2];

		zangle _AngleXOnCenter[NUM_CUBES];
		zangle _AngleYOnCenter[NUM_CUBES];
		zangle _AngleZOnCenter[NUM_CUBES];
#define NUM_CUBES_ROT_ON_CENTER NUM_CUBES

		zangle _AngleInOrbit[NUM_CUBES - 1];
		zangle _AngleOfOrbit[NUM_CUBES - 1];
#define NUM_CUBES_IN_ORBIT (NUM_CUBES - 1)

		byte _CubeColor[NUM_CUBES];

	public:

        CubesObject(void);

        ~CubesObject(void) {}

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // CUBES_H
