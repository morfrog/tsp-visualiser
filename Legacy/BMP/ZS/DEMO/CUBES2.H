/***************************************************************************/
/*                                                                         */
/*                                 CUBES2.H                                */
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


#ifndef CUBES2_H
#define CUBES2_H


#include "animobj.h"


enum Cubes2StateType
{
	CUBES2_FADE_IN,
	CUBES2_TIMING,
	CUBES2_FADE_OUT
};


class Cubes2Object : public AnimationObject
{
	private:

#define FACES_PER_CUBE 6
#define POINTS_PER_FACE 4
#define POINTS_PER_CUBE 8
#define NUM_CUBES 9
		z3DPoint _WorkingCube[POINTS_PER_CUBE];
		z2DPoint _PlotPoints[POINTS_PER_CUBE];
		z2DPoint _FacePoints[POINTS_PER_FACE];

		// keep track of used area in each frame
		z2DPoint _TLDirtyRect[2];
		z2DPoint _BRDirtyRect[2];

		zangle _AngleXOnCenter[NUM_CUBES];
		zangle _AngleYOnCenter[NUM_CUBES];
		zangle _AngleZOnCenter[NUM_CUBES];
#define NUM_CUBES_ROT_ON_CENTER NUM_CUBES

		zangle _AngleInOrbit[NUM_CUBES - 1];
		zangle _AngleOfOrbit[NUM_CUBES - 1];
#define NUM_CUBES_IN_ORBIT (NUM_CUBES - 1)

		byte _CubeColor[NUM_CUBES];

		Cubes2StateType _State;
		long _StateVariable;
		zPalette _Palette;

		char* _TimerPrompt;
		z2DPoint _TimerPosition;
		long _TimerCharPos;

		dword _FrameCount;

	public:

        Cubes2Object(void);

        ~Cubes2Object(void);

		byte Initialize(void);

		byte Animate(void);
};


//===========================================================================
#endif // CUBES2_H
