/***************************************************************************/
/*                                                                         */
/*                                  MAIN.C                                 */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This module provides the main function with ZSVGA initialization for the
| example and demo code provided the ZSVGA graphics library.
|
| Revision History:
|
| Aug 19, 1995: First pass complete
| Sep 26, 1995: Updated function names
\--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include "zsvga.h"

#include "zgetzero.h"
#include "zwhvesa.h"


extern void Example(void);


/*---------------------------------------------------------------------------
| main()
|
| Initializes the ZSVGA graphics library and calls the example.  This
| requires four steps with a fifth optional step:
|
| 1. Allocate a 16k buffer for ZSVGA.
|
| 2. Obtain a near pointer to physical zero, ie. the first megabyte.
|
| 3. Call zInitZSVGA() to setup the library.
|
| 4. Call zWhichSVGA() to identify the video card.
|
| 5. (Optional) Call zWhichVESA() to check for VESA support in case the video
|    card was not identified in step 4.
|
| Just prior to exiting the program, the physical zero pointer, the 16K
| buffer, and any other buffers should be deallocated.
*/
void main(void)
{
	zbyte *pBaseZero;
    zdword cBufSize;
	zbyte *pTempBuf;
    zbyte *pVESAInfo = NULL;
    zbyte ZSVGAReady = 0;

    // allocate working buffer for ZSVGA
    cBufSize = 1024 * 16;
	pTempBuf = (zbyte *)malloc(cBufSize);
	if (!pTempBuf)
	{
		printf("ERROR: Allocating memory for tmp buffer\n");
		exit(1);
	}

    // obtain pointer to lower megabyte of memory
    pBaseZero = (zbyte *)zGetPhysicalZero();

    // initialize the ZSVGA library
    zInitZSVGA(pBaseZero,pTempBuf,cBufSize);

    // identify the video card either in native mode or thru VESA calls
	ZSVGAReady = zWhichSVGA();
    if(!ZSVGAReady)
    {
        /* If zWhichSVGA() was unsuccessful, give VESA a try. */
        pVESAInfo = (zbyte*)malloc(zVESAINFOSIZE);
       	zWhichVESA(pVESAInfo, pBaseZero);
        ZSVGAReady = zSetVESA(pVESAInfo);
    }

    // if everything worked, run the example
    if(ZSVGAReady)
    {
        Example();
    }
    else
    {
        printf("Unidentified video card\n");
    }

    // free pBaseZero and any allocated memory
    zDisablePhysicalZero();
    if(pVESAInfo)
        free(pVESAInfo);
    free(pTempBuf);
}
