/***************************************************************************/
/*                                                                         */
/*                                ZTYPES.H                                 */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is confidential.  It should not be released to */
/* the general public without proper consent.                              */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains common data types and structure definitions for
| Zephyr Software graphics libraries.
|
| Revision History:
|
| Jul 30, 1995: First pass complete
| Nov 04, 1995: Changed base types to defines
\--------------------------------------------------------------------------*/


#ifndef ZTYPES_H
#define ZTYPES_H


/*=========================================================================*\
| Base Types
\*=========================================================================*/

#define zchar   char
#define zbyte   unsigned char
#define zshort  short
#define zword   unsigned short
#define zlong   long
#define zdword  unsigned long


/*=========================================================================*\
| Extended types
\*=========================================================================*/


/*---------------------------------------------------------------------------
| Typedef a 16.16 fixed point format.  Integer math is still very much faster
| than floating point math.
*/
typedef zlong           z1616fix;


/*---------------------------------------------------------------------------
| Typedef an angle for clarity and for future expansion.  Angles are in 16.16
| fixed point format where the range 0-1 corresponds to 0-360 degrees.
| Therefore, 0x4000 is 90 degrees, 0x8000 is 180 and 0x10000 is 360.
*/
typedef z1616fix        zangle;


/*---------------------------------------------------------------------------
| Define the standard true-false values.
*/
typedef enum tag_zBoolean
{
        zFALSE = 0,
        zTRUE
} zBoolean;


/*---------------------------------------------------------------------------
| A two dimensional Cartesian coordinate.  As 16.16 fixed point integer math
| is used in the transformation functions, only values between -32768 and
| +32767 are significant.  Values outside this range are subject to overflow.
*/
typedef struct tag_z2DPoint
{
        zlong x;
        zlong y;
} z2DPoint;


/*---------------------------------------------------------------------------
| A three dimensional Cartesian coordinate.  As 16.16 fixed point integer
| math is used in the transformation functions, only values between -32768
| and +32767 are significant.  Values outside this range are subject to
| overflow.
*/
typedef struct tag_z3DPoint
{
        zlong x;
        zlong y;
        zlong z;
} z3DPoint;


/*---------------------------------------------------------------------------
| Data structure of a bitmap font.  The maximum allowed width and height is
| 8 and 16, respecitvely.  The data is bit packed, 1 bit per pixel, 16 bytes
| per character, regardless of the font's dimensions.
*/
typedef struct tag_zFont
{
        zbyte Width;
        zbyte Height;
        zbyte Data[4096];
} zFont;


/*---------------------------------------------------------------------------
| Data structure of a simple image block.  The image is not compressed.
*/
typedef struct tag_zRasterBlock
{
        zdword Width;
        zdword Height;
        zbyte Data[1];
} zRasterBlock;


/*---------------------------------------------------------------------------
| Logical operation used by functions that write pixels.
*/
typedef enum tag_zPixelMode
{
        zNO_ACTION = 0,
        zSET,
        zXOR,
        zOR,
        zAND
} zPixelMode;


/*=========================================================================*/
#endif /* ZTYPES_H */
