/***************************************************************************/
/*                                                                         */
/*                                ZMOUSE.C                                 */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is released to the public domain.  It is       */
/* provided AS-IS with the simple statement, "We tried it and it worked."  */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| Provide a sample implementation of a mouse library for use with either the
| Symantec or Watcom compiler and the ZSVGA graphics library.  The Borland
| compiler does not appear to support interrupt 0x33.  The Symantec version
| is based on the mouse library provided with the compiler.  The Watcom
| implementation is derived from the etch.c file found on ftp.watcom.com.
|
| Identifiers with a preceeding underscore ('_') indicate they are "hidden"
| and should not be accessed directly.
|
| Revision History:
|
| Sep 19, 1995: First pass complete
\--------------------------------------------------------------------------*/


#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SC__
#include <msmouse.h>
#endif

#include "zsvga.h"
#include "zmouse.h"


/*---------------------------------------------------------------------------
| Define the bitflags for the mouse's status.
*/
#define _zNOMASKS 0x00
#define _zMOVED 0x01
#define _zLEFTPRESSED 0x02
#define _zLEFTRELEASED 0x04
#define _zRIGHTPRESSED 0x08
#define _zRIGHTRELEASED 0x10
#define _zMIDDLEPRESSED 0x20
#define _zMIDDLERELEASED 0x40
#define _zALLMASKS 0x7F
#define _zCURSORVISIBLE 0x40000000
#define _zMOUSEREADY 0x80000000
#define _zMOUSE_NUM_EVENTS 7


/*=========================================================================*\
| Module Variables                                                          |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| The handler function requires a small stack.  Stack usage should be kept to
| a minimum.
*/
static zbyte _zMouseStack[256];
#define _zMOUSE_STACK_PTR (_zMouseStack + sizeof(_zMouseStack))
#ifdef __WATCOMC__
static void far *_zMouseStackSave;
#endif


/*---------------------------------------------------------------------------
| The mouse cursor is draw via the sprite functions.  Therefore, a
| transparent color must be used and the current bank must be saved in case
| the interrupt occurs in the middle of another drawing function.
*/
#define _zMOUSE_TRANS_COLOR 255
static zdword _zMouseBankSave;


/*---------------------------------------------------------------------------
| Store current mouse characteristics.
*/
static zdword _zMouseStatus = _zNOMASKS;// holds button status
static z2DPoint _zMouseCursorPos;   	// position for drawing only
static z2DPoint _zMousePos;         	// position of hot spot
static z2DPoint _zMouseTopLeft;
static z2DPoint _zMouseBottomRight;


/*---------------------------------------------------------------------------
| Storage for mouse cursor.
*/
#define _zMOUSECURSORWIDTH 16
#define _zMOUSECURSORHEIGHT 24
static zRasterBlock *_zMouseCursor;
static z2DPoint _zMouseHotSpot;
static zbyte _zMouseCursorBack[_zMOUSECURSORWIDTH * _zMOUSECURSORHEIGHT + 8];


/*---------------------------------------------------------------------------
| Define the default mouse cursor.
*/
static z2DPoint _zMouseHotSpotDef = { 1, 1};
static zbyte _zMouseCursorDef[] = { _zMOUSECURSORWIDTH-1, 0, 0, 0, _zMOUSECURSORHEIGHT-1, 0, 0, 0,
				      0,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				      0, 15,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,
				      0, 15, 15,  0,255,255,255,255,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15,  0,255,255,255,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15, 15,  0,255,255,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15, 15, 15,  0,255,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15, 15, 15, 15,  0,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15, 15,  0,  0,255,255,255,255,255,255,255,255,255,
				      0, 15, 15, 15, 15,  0,255,255,255,255,255,255,255,255,255,255,
				      0, 15,  0,  0, 15,  0,255,255,255,255,255,255,255,255,255,255,
				      0, 15,  0,  0, 15, 15,  0,255,255,255,255,255,255,255,255,255,
				      0,  0,255,255,  0, 15,  0,255,255,255,255,255,255,255,255,255,
				    255,255,255,255,  0, 15, 15,  0,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,  0, 15,  0,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,  0, 15, 15,  0,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,  0, 15,  0,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,  0, 15, 15,  0,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,  0, 15,  0,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,  0,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255 };


/*=========================================================================*\
| Function Prototypes                                                       |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| Prototype the handler function.
*/
#ifdef __WATCOMC__
static void _loadds far _zMouseHandler(unsigned mask, unsigned curposx, unsigned curposy);
#endif
#ifdef __SC__
static void _zMouseHandler(unsigned mask, unsigned state, unsigned curposx, unsigned curposy);
#endif


/*---------------------------------------------------------------------------
| Allow access to a couple of ZSVGA internals.
|
| zCurbk      - current bank setting
| _zNewBank() - changes current bank as specified in EAX.
*/
#ifdef __cplusplus
extern "C" {
#endif
extern zdword zCurbk;
extern void _zNewBank(void);
#ifdef __cplusplus
}
#endif
#ifdef __WATCOMC__
#pragma aux zCurbk "_*"
#pragma aux _zNewBank "_*"
#endif


/*=========================================================================*\
| Function Definitions                                                      |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| zMouseEnable()
|
| Checks for the existence of a mouse driver (ie. mouse.com) and installs the
| handler function and initializes the variables.
*/
zBoolean zMouseEnable(void)
{
    z2DPoint p1, p2;
    #ifdef __WATCOMC__
    	union REGPACK regs;
    #endif

    #ifdef __SC__
        // check for mouse driver
        if(msm_init() != -1)
        	return(zFALSE);

        // install handler
        msm_signal(_zALLMASKS, _zMouseHandler, _zMOUSE_STACK_PTR);
    #endif

    #ifdef __WATCOMC__
	    // check for mouse driver
        memset(&regs, 0, sizeof(union REGPACK));
        regs.x.eax = 0;
        intr(0x33, &regs);
        if(regs.x.eax == 0x00)
        	return(zFALSE);

    	// install handler
        regs.x.eax = 0x0C;
        regs.x.ecx = _zALLMASKS;
    	regs.x.edx = FP_OFF(_zMouseHandler);
    	regs.w.es = FP_SEG(_zMouseHandler);
        intr(0x33, &regs);
    #endif

    _zMouseStatus = _zMOUSEREADY;

    // reset default cursor
    zMouseSetCursor((zRasterBlock*)_zMouseCursorDef, &_zMouseHotSpotDef);

    // reset usable area
    p1.x = zViewX1;
    p1.y = zViewY1;
    p2.x = zViewX2;
    p2.y = zViewY2;
    zMouseSetArea(&p1, &p2);

    return(zTRUE);
}


/*---------------------------------------------------------------------------
| zMouseDisable()
|
| Uninstalls the mouse handler.  This must be done before ending a program
| that has called zMouseEnable().
*/
void zMouseDisable(void)
{
    // uninstall handler and disable mouse functions

    #ifdef __SC__
    	msm_term();
    #endif

    #ifdef __WATCOMC__
    	union REGS regs;

    	regs.w.ax = 0;
        int386(0x33, &regs, &regs);
    #endif

    _zMouseStatus = _zNOMASKS;
}


/*---------------------------------------------------------------------------
| zMouseShow()
|
| Turns on the display of the cursor.
*/
void zMouseShow(void)
{
    if(_zMouseStatus & _zMOUSEREADY)
    {
        if(!(_zMouseStatus & _zCURSORVISIBLE))
    	{
            _zMouseStatus |= _zCURSORVISIBLE;
            zSpriteGAP(_zMOUSE_TRANS_COLOR, &_zMouseCursorPos, _zMouseCursor, (zRasterBlock*)_zMouseCursorBack);
        }
    }
}


/*---------------------------------------------------------------------------
| zMouseHide()
|
| Turns off display of the mouse cursor.
*/
void zMouseHide(void)
{
    if(_zMouseStatus & _zMOUSEREADY)
    {
        if(_zMouseStatus & _zCURSORVISIBLE)
    	{
            _zMouseStatus &= ~_zCURSORVISIBLE;
        	zSpritePut(_zMOUSE_TRANS_COLOR, &_zMouseCursorPos, (zRasterBlock*)_zMouseCursorBack);
        }
    }
}


/*---------------------------------------------------------------------------
| zMouseGetStatus()
|
| Returns the mouse's current button status and hot spot position.
*/
void zMouseGetStatus(zdword *Status, z2DPoint *Position)
{
    if(_zMouseStatus & _zMOUSEREADY)
    {
    	if(Status)
        	*Status = _zMouseStatus;

    	if(Position)
            *Position = _zMousePos;
    }
}


/*---------------------------------------------------------------------------
| zMouseSetDefaultCursor()
|
| Sets the cursor to the default shape as stored internally.
*/
zBoolean zMouseSetDefaultCursor(void)
{
    return(zMouseSetCursor((zRasterBlock*)_zMouseCursorDef, &_zMouseHotSpotDef));
}


/*---------------------------------------------------------------------------
| zMouseSetCursor()
|
| Sets the cursor to a user defined shape.  The memory block holding the
| cursor should not be freed while the cursor is in use.
*/
zBoolean zMouseSetCursor(zRasterBlock *Cursor, z2DPoint *HotSpot)
{
    if(_zMouseStatus & _zMOUSEREADY)
    {
        if( (HotSpot->x >= 0) && (HotSpot->x < _zMOUSECURSORWIDTH) &&
            (HotSpot->y >= 0) && (HotSpot->y < _zMOUSECURSORHEIGHT) &&
            (Cursor->Width == (_zMOUSECURSORWIDTH-1)) && (Cursor->Height == (_zMOUSECURSORHEIGHT-1)) )
    	{
        	_zMouseHotSpot = *HotSpot;
        	_zMouseCursor = Cursor;
        	return(zTRUE);
    	}
    }
    return(zFALSE);
}


/*---------------------------------------------------------------------------
| zMouseSetPosition()
|
| Sets the cursor's position as specified.
*/
zBoolean zMouseSetPosition(z2DPoint *Pos)
{
    union REGS regs;

    if(_zMouseStatus & _zMOUSEREADY)
    {
        if( (Pos->x > _zMouseTopLeft.x) && (Pos->x < _zMouseBottomRight.x) &&
            (Pos->y > _zMouseTopLeft.y) && (Pos->y < _zMouseBottomRight.y) )
        {
            #ifdef __SC__
	            regs.e.eax = 4;
    	        regs.e.ecx = Pos->x;
        	    regs.e.edx = Pos->y;
                int86(0x33, &regs, &regs);
            #endif
            #ifdef __WATCOMC__
                regs.x.eax = 4;
                regs.x.ecx = Pos->x;
                regs.x.edx = Pos->y;
                int386(0x33, &regs, &regs);
            #endif

            _zMousePos = *Pos;
            _zMouseCursorPos = *Pos;
            _zMouseCursorPos.x -= _zMouseHotSpot.x;
            _zMouseCursorPos.y -= _zMouseHotSpot.y;
            return(zTRUE);
        }
    }
    return(zFALSE);
}


/*---------------------------------------------------------------------------
| zMouseSetArea()
|
| Defines the cursor's area of motion to the specified rectangle.  The
| rectangle must be entirely on the screen.  The cursor is also moved to the
| center of the new area.
*/
zBoolean zMouseSetArea(z2DPoint *TopLeft, z2DPoint *BottomRight)
{
    union REGS regs;
    z2DPoint center;

    if(_zMouseStatus & _zMOUSEREADY)
    {
        if( (TopLeft->x < zViewX1) || (BottomRight->x < zViewX1) ||
            (TopLeft->x > zViewX2) || (BottomRight->x > zViewX2) ||
            (TopLeft->y < zViewY1) || (BottomRight->y < zViewY1) ||
            (TopLeft->y > zViewY2) || (BottomRight->y > zViewY2) )
        {
            return(zFALSE);
        }

	    if(TopLeft->x <= BottomRight->x)
    	{
        	_zMouseTopLeft.x = TopLeft->x;
        	_zMouseBottomRight.x = BottomRight->x;
    	}
    	else
    	{
        	_zMouseTopLeft.x = BottomRight->x;
        	_zMouseBottomRight.x = TopLeft->x;
    	}

    	if(TopLeft->y <= BottomRight->y)
    	{
        	_zMouseTopLeft.y = TopLeft->y;
        	_zMouseBottomRight.y = BottomRight->y;
    	}
    	else
    	{
        	_zMouseTopLeft.y = BottomRight->y;
        	_zMouseBottomRight.y = TopLeft->y;
    	}

    	// set horizontal range
        #ifdef __SC__
	        regs.e.eax = 7;
    	    regs.e.ecx = _zMouseTopLeft.x;
        	regs.e.edx = _zMouseBottomRight.x;
        	int86(0x33, &regs, &regs);
    	#endif
    	#ifdef __WATCOMC__
            regs.x.eax = 7;
            regs.x.ecx = _zMouseTopLeft.x;
            regs.x.edx = _zMouseBottomRight.x;
            int386(0x33, &regs, &regs);
    	#endif

    	// set vertical range
        #ifdef __SC__
	        regs.e.eax = 8;
    	    regs.e.ecx = _zMouseTopLeft.y;
        	regs.e.edx = _zMouseBottomRight.y;
            int86(0x33, &regs, &regs);
    	#endif
    	#ifdef __WATCOMC__
            regs.x.eax = 8;
            regs.x.ecx = _zMouseTopLeft.y;
            regs.x.edx = _zMouseBottomRight.y;
            int386(0x33, &regs, &regs);
    	#endif

    	center.x = (_zMouseTopLeft.x + _zMouseBottomRight.x) >> 1;
    	center.y = (_zMouseTopLeft.y + _zMouseBottomRight.y) >> 1;
        zMouseSetPosition(&center);

        return(zTRUE);
    }
    return(zFALSE);
}


/*---------------------------------------------------------------------------
| _zMouseHandlerGetStack() (Watcom only)
| _zMouseHandlerSetStack() (Watcom only)
| _zMouseHandlerRestoreBank() (Watcom only)
|
| Define the routines for saving and restoring the stack (Watcom only) and
| restoring the current bank.
*/
#ifdef __WATCOMC__
extern void far *_zMouseHandlerGetStack();
#pragma aux _zMouseHandlerGetStack =\
    "MOV DX, SS"                	\
    "MOV EAX, ESP"              	\
    value [ DX EAX ];

extern void _zMouseHandlerSetStack(void far *);
#pragma aux _zMouseHandlerSetStack =\
    "MOV SS, DX"                	\
    "MOV ESP, EAX"              	\
    parm [ DX EAX ];

extern void _zMouseHandlerRestoreBank(void);
#pragma aux _zMouseHandlerRestoreBank =	\
    "MOV  EAX, _zMouseBankSave"         \
    "CALL _zNewBank";
#endif


/*---------------------------------------------------------------------------
| _zMouseHandler()
|
| This handler function is installed in zMouseEnable and is called by any
| mouse action.  The argument 'mask' details the event while curposx and
| curposy give the position of the event.
|
| As this function is called during an interrupt, it must use an independent
| stack.  Also, this interrupt may occur during another ZSVGA drawing
| function.  This requires saving and restoring the current bank setting.
*/
#ifdef __WATCOMC__
#pragma off (check_stack)
static void _loadds far _zMouseHandler(unsigned mask, unsigned curposx, unsigned curposy)
#endif
#ifdef __SC__
static void _zMouseHandler(unsigned mask, unsigned state, unsigned curposx, unsigned curposy)
#endif
{
    #ifdef __WATCOMC__
    #pragma aux _zMouseHandler parm [EAX] [ECX] [EDX]
    #endif

    // save bank and stack (Watcom only)
    #ifdef __WATCOMC__
        _zMouseStackSave = _zMouseHandlerGetStack();
        _zMouseHandlerSetStack(_zMOUSE_STACK_PTR);
    #endif
    _zMouseBankSave = zCurbk;

    // decode mask
    _zMouseStatus &= ~_zALLMASKS;
    _zMouseStatus |= mask;
    if(_zMouseStatus & _zLEFTPRESSED)
        _zMouseStatus |= zLEFTDOWN;
    else if(_zMouseStatus & _zLEFTRELEASED)
        _zMouseStatus &= ~zLEFTDOWN;
    if(_zMouseStatus & _zRIGHTPRESSED)
        _zMouseStatus |= zRIGHTDOWN;
    else if(_zMouseStatus & _zRIGHTRELEASED)
        _zMouseStatus &= ~zRIGHTDOWN;
    if(_zMouseStatus & _zMIDDLEPRESSED)
        _zMouseStatus |= zMIDDLEDOWN;
    else if(_zMouseStatus & _zMIDDLERELEASED)
        _zMouseStatus &= ~zMIDDLEDOWN;

    // erase current cursor if necessary
    if((mask & _zMOVED) && (_zMouseStatus & _zCURSORVISIBLE))
        zSpritePut(_zMOUSE_TRANS_COLOR, &_zMouseCursorPos, (zRasterBlock*)_zMouseCursorBack);

    // curposx and curposy should already be valid because of SetArea
    // recalculate new position
    if(mask & _zMOVED)
    {
        _zMousePos.x = curposx;
        _zMouseCursorPos.x = curposx;
        _zMouseCursorPos.x -= _zMouseHotSpot.x;
        _zMousePos.y = curposy;
        _zMouseCursorPos.y = curposy;
        _zMouseCursorPos.y -= _zMouseHotSpot.y;
    }

    // draw new cursor if necessary
    if((mask & _zMOVED) && (_zMouseStatus & _zCURSORVISIBLE))
        zSpriteGAP(_zMOUSE_TRANS_COLOR, &_zMouseCursorPos, _zMouseCursor, (zRasterBlock*)_zMouseCursorBack);

    // reset status
    _zMouseStatus &= ~_zALLMASKS;

    // restore original bank and stack (Watcom only)
    #ifdef __SC__
        if(zCurbk != _zMouseBankSave)
        {
            asm MOV  EAX, _zMouseBankSave
            asm CALL _zNewBank
        }
    #endif
    #ifdef __WATCOMC__
	    if(zCurbk != _zMouseBankSave)
    	    _zMouseHandlerRestoreBank();
        _zMouseHandlerSetStack(_zMouseStackSave);
    #endif
}
#ifdef __WATCOMC__
#pragma on (check_stack)
#endif
