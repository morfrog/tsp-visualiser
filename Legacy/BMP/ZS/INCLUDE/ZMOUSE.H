/***************************************************************************/
/*                                                                         */
/*                                ZMOUSE.H                                 */
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
| Revision History:
|
| Sep 19, 1995: First pass complete
| Sep 26, 1995: Update formatting
\--------------------------------------------------------------------------*/


#ifndef ZMOUSE_H
#define ZMOUSE_H

#include "ztypes.h"


/*---------------------------------------------------------------------------
| Define the bitflags for the mouse's status.
*/
#define zLEFTDOWN 0x00010000
#define zRIGHTDOWN 0x00020000
#define zMIDDLEDOWN 0x00040000


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------
| zMouseEnable()
|
| Checks for the existence of a mouse driver (ie. mouse.com) and installs the
| handler function and initializes the variables.
|
| Inputs: none
|
| Return: boolean flag indicating success or failure
*/
extern zBoolean zMouseEnable(void);


/*---------------------------------------------------------------------------
| zMouseDisable()
|
| Uninstalls the mouse handler.  This must be done before ending a program
| that has called zMouseEnable().
|
| Inputs: none
|
| Return: none
*/
extern void zMouseDisable(void);


/*---------------------------------------------------------------------------
| zMouseShow()
|
| Turns on the display of the cursor.
|
| Inputs: none
|
| Return: none
*/
extern void zMouseShow(void);


/*---------------------------------------------------------------------------
| zMouseHide()
|
| Turns off the display of the cursor.
|
| Inputs: none
|
| Return: none
*/
extern void zMouseHide(void);


/*---------------------------------------------------------------------------
| zMouseGetStatus()
|
| Returns the mouse's current button status and hot spot position.
|
| Inputs: Status   - receives the current button status
|         Position - receives the current position of the cursor hot spot
|
| Return: none
*/
extern void zMouseGetStatus(zdword *Status, z2DPoint *Position);


/*---------------------------------------------------------------------------
| zMouseSetDefaultCursor()
|
| Sets the cursor to the default shape as stored internally.
|
| Inputs: none
|
| Return: boolean flag indicating success or failure
*/
extern zBoolean zMouseSetDefaultCursor(void);


/*---------------------------------------------------------------------------
| zMouseSetCursor()
|
| Sets the cursor to a user defined shape.  The memory block holding the
| cursor should not be freed while the cursor is in use.
|
| Inputs: Cursor  - pointer to zRasterBlock holding specified cursor shape
|         HotSpot - pointer to position of hot spot within cursor
|
| Return: boolean flag indicating success or failure
*/
extern zBoolean zMouseSetCursor(zRasterBlock *Cursor, z2DPoint *HotSpot);


/*---------------------------------------------------------------------------
| zMouseSetPosition()
|
| Sets the cursor's position as specified.
|
| Inputs: Pos - pointer to desired position
|
| Return: boolean flag indicating success or failure
*/
extern zBoolean zMouseSetPosition(z2DPoint *Pos);


/*---------------------------------------------------------------------------
| zMouseSetArea()
|
| Defines the cursor's area of motion to the specified rectangle.  The
| rectangle must be entirely on the screen.  The cursor is also moved to the
| center of the new area.
|
| Inputs: TopLeft     - pointer to position of top-left corner of rectangle
|         BottomRight - pointer to position of bottom-right corner of rect
|
| Return: boolean flag indicating success or failure
*/
extern zBoolean zMouseSetArea(z2DPoint *TopLeft, z2DPoint *BottomRight);


#ifdef __cplusplus
}
#endif


/*=========================================================================*/
#endif /* ZMOUSE_H */
