/***************************************************************************/
/*                                                                         */
/*                               DEMOOBJ.H                                 */
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


#ifndef DEMOOBJ_H
#define DEMOOBJ_H


#include <time.h>

#include "common.h"
#include "zsvga.h"


enum ZDOError
{
    ZDO_SUCCESS,
    ZDO_INSUFFICIENT_MEMORY,
    ZDO_NO_VIDEO,
	ZDO_NO_LOGO,
    ZDO_MAX_ERRORS
};


enum ZDOResolution
{
    ZDO_RES_640x400,
    ZDO_RES_640x480,
    ZDO_RES_800x600,
    ZDO_RES_1024x768,
    ZDO_RES_1280x1024
};


enum ZDOChoice
{
    ZDO_NO_CHOICE,
	ZDO_QUIT,
    ZDO_RUN_POINTS,
    ZDO_RUN_LINES,
    ZDO_RUN_AALINES,
    ZDO_RUN_CURVES,
    ZDO_RUN_SOLIDS,
    ZDO_RUN_STRINGS,
    ZDO_RUN_BEZIERS,
	ZDO_RUN_SPRITES,
	ZDO_RUN_SQUARES,
	ZDO_RUN_CUBES,
	ZDO_RUN_CUBES2,
    ZDO_RUN_ALL,
    ZDO_MAX_CHOICES
};


extern z2DPoint ZeroZero;
extern z2DPoint *pTLScreen;
extern z2DPoint *pBRScreen;


char* GetSVGAString(zSVGAChipset Chip);


/*=========================================================================*\
| ZSVGADemoObject
|
| This class controls the demonstration of the Zephyr Software ZSVGA graphics
| library.  Usage:
| 1) Instatiate: ZSVGADemoObject TheDemo;
| 2) Initialize: TheDemo.Initialize();
| 3) Introduce:  TheDemo.Introduce();
| 4) Execute:    TheDemo.Run();
\*=========================================================================*/
class ZSVGADemoObject
{
    private:

        ZDOError _ErrorLevel;
        byte* _pBaseZero;
        void* _pTmpBuf;
        zSVGAChipset _NativeSupport;
		dword _KBVideoMemory;
		void* _pVESAInfo;
	    dword _VESASupport;
        ZDOResolution _Resolution;

		ZDOChoice GetChoice(void);

    public:

       	ZSVGADemoObject(void);

        ~ZSVGADemoObject(void);

        byte Initialize(int argc, char** argv);

        void Introduce(void);

        byte Run(void);
};


//===========================================================================
#endif /* DEMOOBJ_H */
