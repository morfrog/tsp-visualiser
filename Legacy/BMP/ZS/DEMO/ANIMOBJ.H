/***************************************************************************/
/*                                                                         */
/*                                ANIMOBJ.H                                */
/*                                                                         */
/***************************************************************************/
/*      (c) 1996 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Prototype for a base class of an animating object.
|
| Revision History:
|
| Jan 17, 1996: First pass complete.
\--------------------------------------------------------------------------*/


#ifndef ANIMOBJ_H
#define ANIMOBJ_H


#include <stdio.h>


#include "common.h"
#include "zsvga.h"


enum AnimObjType
{
    AO_BASE,
    AO_PALETTECURTAIN,
    AO_RANDPOINTS,
    AO_RANDLINES,
    AO_RANDAALINES,
    AO_RANDCURVES,
    AO_RANDSOLIDS,
    AO_RANDSTRINGS,
    AO_RANDBEZIERS,
	AO_SPRITES,
    AO_SQUARES,
    AO_CUBES,
    AO_CUBES2,
    AO_MAX_TYPES
};


/*=========================================================================*\
| AnimationObject
|
| A base class for an animating object.
\*=========================================================================*/
class AnimationObject
{
	protected:

		byte _Ready;
		char* _Prompt;
		z2DPoint _PromptPosition;

    public:

        AnimationObject(void)
		{
			_Prompt = NULL;
            _Ready = FALSE;
        }

		virtual ~AnimationObject(void)
		{
			_Ready = FALSE;
		}

		virtual byte Initialize(void)
		{
			_Ready = TRUE;
			return(TRUE);
		}

		virtual byte Animate(void)
		{
			return(FALSE);
		}
};


AnimationObject* NewAnimObject(AnimObjType Type);


//===========================================================================
#endif // ANIMOBJ_H
