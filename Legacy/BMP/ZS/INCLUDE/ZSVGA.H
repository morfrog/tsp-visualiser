/***************************************************************************/
/*                                                                         */
/*                                 ZSVGA.H                                 */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is confidential.  It should not be released to */
/* the general public without proper consent.                              */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains the function prototypes and structure definitions for
| the Zephyr Software protected Mode graphics library.
|
| Revision History:
|
| Jan 23, 1996: First release ready
\--------------------------------------------------------------------------*/


#ifndef ZSVGA_H
#define ZSVGA_H


#include "ztypes.h"


/*---------------------------------------------------------------------------
| HOW TO USE THE ZEPHYR SOFTWARE ZSVGA GRAPHICS LIBRARY
|
| Initializing the ZSVGA graphics library requires four steps with an
| optional fifth step.  To see the initialization process in use, refer to
| the sample files suplment\main.c and demo\demoobj.cpp.  The steps are:
|
| 1. Allocate a 16k buffer for ZSVGA.
|
| 2. Obtain a near pointer to physical zero, ie. the first megabyte.  See
|    the file suplment\zgetzero.c for details.
|
| 3. Call zInitZSVGA() to setup the library.
|
| 4. Call zWhichSVGA() to identify the video card.
|
| 5. (Optional) Call zWhichVESA() to check for VESA support in case the video
|    card was not identified in step 4.  See the file suplment\zwhvesa.c for
|    additional information.
|
| The library is now ready for use.  Just prior to exiting the program, the 
| physical zero pointer, the 16K buffer, and any other buffers should be 
| deallocated.
*/

/*---------------------------------------------------------------------------
| The recommended size of the working buffer the ZSVGA Graphics Library
| requires.  See zInitZSVGA() and zSetTempBuffer().
*/
#define zRECOMMENDED_BUFFER_SIZE (16 * 1024)


/*---------------------------------------------------------------------------
| The basic RGB triplet.  Remember that SVGAs use a 6 bit palette.  Only the
| lower 6 bits are significant.  Also, the alpha byte is nonfunctional and
| ignored.  It's only purpose is to establish 32 bit alignment.
*/
typedef struct tag_zRGB
{
    zbyte r;
    zbyte g;
    zbyte b;
    zbyte a;
} zRGB;


/*---------------------------------------------------------------------------
| The standard 256 color palette.
*/
typedef struct tag_zPalette
{
	zRGB Color[256];
} zPalette;


/*---------------------------------------------------------------------------
| Identification codes for the chipset on the SVGA card.
*/
typedef enum tag_zSVGAChipset
{
	zUNKNOWN = 0,
	zACUMOS,
	zATI,
	zAHEADA,
	zAHEADB,
	zCHIPSTECH,
	zCIRRUS,
	zEVEREX,
	zGENOA,
	zNCR,
	zOAKTECH,
	zPARADISE,
	zREALTEK,
	zTRIDENT,
	zTSENG3,
	zTSENG4,
	zS3,
	zVIDEO7,
	zAVANCE,
	zMXIC,
	zPRIMUS,
	zACER,
	zUMC
} zSVGAChipset;


/*---------------------------------------------------------------------------
| Information structure detailing the positional status of the joysticks.
| The first two bits of JstkAButs and JstkBButs describe the two buttons on
| each joystick.  Bit 0 is the first button; bit 1 the second.  The bit will
| be set when the button is pressed.
*/
typedef struct tag_zJstkStatus
{
	zlong JstkAX;
	zlong JstkAY;
	zlong JstkAButs;
	zlong JstkBX;
	zlong JstkBY;
	zlong JstkBButs;
} zJstkStatus;


#ifdef __cplusplus
extern "C" {
#endif


/*=========================================================================*\
| Global Variables                                                          |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| zMaxX and zMaxY define the dimensions of the active display page.  zMaxX
| should not be modified.  However, by changing zMaxY enlongated screens can
| be created for easy scrolling zPageDisplay.  These values are set by a call
| to any zRes function.
*/
extern const zdword zMaxX;
extern zdword zMaxY;


/*---------------------------------------------------------------------------
| These variables define the current clipping box.  They are set by a call to
| any zRes function or zSetView and should not otherwise be modified.
*/
extern const zdword zViewX1;
extern const zdword zViewY1;
extern const zdword zViewX2;
extern const zdword zViewY2;


/*=========================================================================*\
| Function Prototypes                                                       |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| z2DRotate()
|
| Takes the two dimensional points given in InArray and rotates them about 
| the given center.  The results are returned in OutArray which can be the 
| same as InArray.  A positive angle causes a clockwise rotation on the 
| screen, from the positive X axis to the positive Y axis.  The function 
| assumes space for OutArray has been properly allocated.
|
| Inputs: NumPoints - number of points to be rotated
|         Origin    - center of rotation
|         Angle     - angle of rotation about center
|         InArray   - pointer to buffer containing points to rotate
|         OutArray  - pointer to buffer to receive rotated points
|
| Return: none
*/
extern void z2DRotate(zdword NumPoints, z2DPoint *Origin, zangle Angle, z2DPoint *InArray, z2DPoint *OutArray);


/*---------------------------------------------------------------------------
| z2DScale()
|
| Multiplies each coordinate in the two dimensional array InArray by the 
| corresponding scale factor ScaleX or ScaleY.  The results are stored in 
| OutArray which can be the same as InArray.  The scale factors are in 16.16 
| fixed point format.
|
| Inputs: NumPoints      - number of points to be scaled
|         ScaleX, ScaleY - fixed point scale factors
|         InArray        - pointer to buffer containing points to be scaled
|         OutArray       - pointer to buffer to receive scaled points
|
| Return: none
*/
extern void z2DScale(zdword NumPoints, z1616fix ScaleX, z1616fix ScaleY, z2DPoint *InArray, z2DPoint *OutArray);


/*---------------------------------------------------------------------------
| z2DTranslate()
|
| Takes the two dimensional points given in InArray and translates them by 
| the specified number of pixels along each axis.  The results are returned 
| in OutArray which can be the same as InArray.  The function assumes that 
| space for OutArray has been properly allocated.
|
| Inputs: NumPoints - number of points to translate
|         Offset    - translation offsets
|         InArray   - pointer to buffer containing points to translate
|         OutArray  - pointer to buffer to receive translated points
|
| Return: none
*/
extern void z2DTranslate(zdword NumPoints, z2DPoint *Offset, z2DPoint *InArray, z2DPoint *OutArray);


/*---------------------------------------------------------------------------
| z3DProject()
|
| Projects the specified number of three dimensional points in InArray into 
| two dimensions.  The two dimensional points are stored in OutArray and the 
| function assumes memory for OutArray has been properly allocated.  The
| location of the viewer in three dimensional space is at the origin (0, 0, 
| 0).  The viewer is facing down the positive Z axis.  The view screen is 
| defined by the current clipping viewport and is centered on the Z axis with
| a Z location specified by ScreenPos.  If a point is in the X-Y plane 
| (Z = 0) or is located behind the viewer (Z < 0), z3DProject() returns a 
| zero indicating one or more failed points.  The X and Y projected values of
| failed points will both be set to 0x80000000 to make them easily 
| identified.
|
| Inputs: NumPoints - number of points to project
|         ScreenPos - position of the screen along the positive Z axis
|         InArray   - pointer to buffer containing 3D points to be projected
|         OutArray  - pointer to buffer to receive the 2D projected points
|
| Return: one if successul, zero if one or more points did not project
*/
extern zlong z3DProject(zdword NumPoints, zlong ScreenPos, z3DPoint *InArray, z2DPoint *OutArray);


/*---------------------------------------------------------------------------
| z3DRotate()
|
| Takes the three dimensional points given in InArray and rotates them by the 
| specified angles, AngleZ, AngleY, and AngleX, about their respective axes.  
| The rotation order is the same as the order the angles appear in the 
| argument list.  A positive AngleZ causes a rotation from the positive X 
| axis to the positive Y axis.  The results are returned in OutArray which 
| can be the same as InArray.  The function assumes space for OutArray has 
| been properly allocated.
|
| Inputs: NumPoints       - number of points to be rotated
|         Origin          - center of rotation
|         AngleZ, AngleY, AngleX - angles of rotation about each axis
|         InArray         - pointer to buffer containing points to rotate
|         OutArray        - pointer to buffer to receive rotated points
|
| Return: none
*/
extern void z3DRotate(zdword NumPoints, z3DPoint *Origin, zangle AngleZ, zangle AngleY, zangle AngleX, z3DPoint *InArray, z3DPoint *OutArray);


/*---------------------------------------------------------------------------
| z3DScale()
|
| Multiplies each coordinate in the three dimensional array InArray by the 
| corresponding scale factor ScaleX, ScaleY or ScaleZ.  The results are 
| stored in OutArray which can be the same as InArray.  The scale factors are
| in 16.16 fixed point format.
|
| Inputs: NumPoints       - number of points to be scaled
|         ScaleX, ScaleY, ScaleZ - fixed point scale factors
|         InArray         - pointer to buffer containing points to be scaled
|         OutArray        - pointer to buffer to receive scaled points
|
| Return: none
*/
extern void z3DScale(zdword NumPoints, z1616fix ScaleX, z1616fix ScaleY, z1616fix ScaleZ, z3DPoint *InArray, z3DPoint *OutArray);


/*---------------------------------------------------------------------------
| z3DTranslate()
|
| Takes the three dimensional points given in InArray and translates them by 
| the specified number of pixels along each axis.  The results are returned 
| in OutArray which can be the same as InArray.  The function assumes that 
| space for OutArray has been properly allocated.
|
| Inputs: NumPoints - number of points to translate
|         Offset    - translation offsets
|         InArray   - pointer to buffer containing points to translate
|         OutArray  - pointer to buffer to receive translated points
|
| Return: none
*/
extern void z3DTranslate(zdword NumPoints, z3DPoint *Offset, z3DPoint *InArray, z3DPoint *OutArray);


/*---------------------------------------------------------------------------
| zBlockGet()
|
| Stores the pixel data contained within the block defined by P1-P2 in the
| buffer referenced by GfxBlk.  Memory for GfxBlk must be allocated as a
| zRasterBlock structure with a size in bytes equal to the pixel area of the
| block plus eight bytes for the width and height integers of the
| zRasterBlock structure.
|
| Note, however, that GfxBlk can be quite large.  If the size of GfxBlk is
| insufficient, zBlockGet() will overwrite any data in memory contained
| beyond GfxBlk and may possibly cause the system to crash. zBlockGet()
| enforces (P2->x) >= (P1->x) and (P2->y) >= (P1->y).  Also, the coordinates
| must be valid on the screen at the current resolution.
|
| Inputs: P1, P2 - locations of upper, left and lower, right corners of block
|         GfxBlk - system memory to receive the image block
|
| Return: one for success, zero if the coordinates are invalid
*/
extern zlong zBlockGet(z2DPoint *P1, z2DPoint *P2, zRasterBlock *GfxBlk);


/*---------------------------------------------------------------------------
| zBlockPut()
|
| Places the pixel data contained in the variable referenced by GfxBlk on the
| screen.  The top, left corner of the block is specified by P.  Any (P->x,
| P->y) is acceptable and any portion of the block that lies outside of the 
| currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         P      - location on the active screen of the upper, left corner
|         GfxBlk - system memory holding the image block
|
| Return: none
*/
extern void zBlockPut(zPixelMode Mode, z2DPoint *P, zRasterBlock *GfxBlk);


/*---------------------------------------------------------------------------
| zBlockResize()
|
| Takes the bitmap in SrcGfxBlk and scales it up or down according to the 
| dimensions given in DestGfxBlk.  The resulting bitmap is returned in 
| DestGfxBlk which should already be declared with an appropriate size.
|
| Inputs: SrcGfxBlk  - the original image block
|         DestGfxBlk - the new block with dimensions specifying new size
|
| Return: none
*/
extern void zBlockResize(zRasterBlock *SrcGfxBlk, zRasterBlock *DestGfxBlk);


/*---------------------------------------------------------------------------
| zBlockRotate()
|
| Rotates the bitmap in SrcGfxBlk by the number of degrees specified by
| Angle.  Blank space around the newly rotated block is filled with the color
| given by BackFill.  The resulting bitmap is stored in DestGfxBlk.  The size
| of DestGfxBlk should be at least as big as given by zBlockRotateSize().
|
| The bitmap rotation algorithm is a three-pass shear technique modified to 
| make efficient use of this library's internal buffers.  This algorithm 
| requires a large temporary buffer with a size specified by
| zBlockRotateSize().  Use zSetTempBuffer() to provide an appropriate buffer.
|
| Inputs: Angle      - the angle rotation
|         BackFill   - color to fill in the unoccupied pixels of the new
|                      image
|         SrcGfxBlk  - the original image block
|         DestGfxBlk - the newly rotated image
|
| Return: one if successful, zero otherwise
*/
extern zlong zBlockRotate(zangle Angle, zbyte BackFill, zRasterBlock *SrcGfxBlk, zRasterBlock *DestGfxBlk);


/*---------------------------------------------------------------------------
| zBlockRotateSize()
|
| Takes the bitmap in SrcGfxBlk and calculates the required size of the
| output and temporary buffers needed when zBlockRotate() is called.  
| zBlockRotateSize() should be called prior to zBlockRotate() to insure that 
| buffer integrity is maintained.
|
| Inputs: Angle     - the angle rotation
|         SrcGfxBlk - the original image block
|         TempSize  - receives size of necessary temporary buffer
|         FinalSize - receives size of rotated image block
|
| Return: one if internal buffer is already of sufficient size, zero if it
|         must be adjusted for the actual rotation
*/
extern zdword zBlockRotateSize(zangle Angle, zRasterBlock *SrcGfxBlk, zdword *TempSize, zdword *FinalSize);


/*---------------------------------------------------------------------------
| zByteCopy()
|
| Copies the specified number of bytes from Src to Dst.  It assumes that
| space for Dst has been properly allocated.  It is much faster than using a
| for loop or memcpy().
|
| Inputs: Src      - pointer to buffer to be copied
|         Dst      - pointer to buffer to receive copy
|         NumBytes - number of bytes to copy
|
| Return: none
*/
extern void zByteCopy(void *Src, void *Dst, zdword NumBytes);


/*---------------------------------------------------------------------------
| zDrawAALine()
|
| Draws an antialiased line of the specified color with endpoints located at 
| P1 and P2.  Antialiased lines trick the eye into seeing true vector lines 
| instead of a jagged series of individual pixels.  This is accomplished by 
| drawing two pixels of the same color but with different intensities 
| (brightness) for each pixel drawn by zDrawAALine().  The eye averages the 
| pixels and sees only a straight line.  Note that the palette must be set up 
| with the correct color entries for the line to look correct.  Any portion 
| of the line that lies outside of the currently defined viewport will not be 
| drawn.  NumColorBits should be a number in the range of 0 to 6 specifying 
| the number of shades of color in the palette (2^0 = 1 is one color shade; 
| 2^6= 64 is 64 shades of color).  These shades should be contiguous in the 
| palette from index Color to index (Color + 2^NumColorBits - 1) starting 
| with the full desired brightness.  The best compromise of detail versus 
| palette entries is about 3 (2^3=8 is 8 shades, or palette entries, of 
| color).
|
| Inputs: NumColorBits - power-of-two shades of Color available
|         Color        - base color of line
|         P1, P2       - endpoints of line
|
| Return: none
*/
extern void zDrawAALine(zlong NumColorBits, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zDrawBox()
|
| Draws a rectangle with the specified color using Mode with opposite 
| vertices defined by P1 and P2.  The vertices given do not need to be 
| exactly the top left and bottom right.  They only have to reference 
| opposite vertices of the rectangle.  Any portion of the rectangle that lies
| outside of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of box
|         P1, P2 - diagonal corners of box
|
| Return: none
*/
extern void zDrawBox(zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zDrawCirArc()
|
| Draws a circular arc of the specified color and mode with the center 
| located at Center, radius of Radius, starting at the angle specified by 
| StartAngle and ending at the angle specified by EndAngle.  However, 
| StartAngle must be less than EndAngle, ie. EndAngle may be larger than 
| 0x10000.  Any portion of the arc that lies outside of the currently defined
| viewport will not be drawn.
|
| Inputs: Mode                 - pixel write mode
|         Color                - color of arc
|         Center               - location of center of base circle
|         Radius               - radius of base circle
|         StartAngle, EndAngle - angles defining sweep of arc
|
| Return: none
*/
extern void zDrawCirArc(zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius, zangle StartAngle, zangle EndAngle);


/*---------------------------------------------------------------------------
| zDrawCircle()
|
| Draws a circle of the specified color using Mode with the center located at 
| Center and a radius of Radius.  Any portion of the circle that lies outside 
| of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of circle
|         Center - location of center of circle
|         Radius - radius of circle
|
| Return: none
*/
extern void zDrawCircle(zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius);


/*---------------------------------------------------------------------------
| zDrawCubicBezier()
|
| Draws a standard cubic Bezier curve connecting the points POn1 and POn2 
| using the specified mode and color.  The off-curve control points are POff1 
| and POff2.  A technical description of cubic Bezier curves is beyond the 
| scope of this manual.  However, most advanced computer graphics texts 
| contain discussions on this topic.
|
| Inputs: Mode                     - pixel write mode
|         Color                    - color of curve
|         POn1, POff1, POff2, POn2 - the control points defining the curve
|
| Return: none
*/
extern void zDrawCubicBezier(zPixelMode Mode, zbyte Color, z2DPoint *POn1, z2DPoint *POff1, z2DPoint *POff2, z2DPoint *POn2);


/*---------------------------------------------------------------------------
| zDrawEllArc()
|
| Draws an elliptical arc of the specified color and mode with the center 
| located at Center.  The radius in the horizontal direction is Raduis->x and
| the radius in the vertical direction is Radius->y.  The starting angle is 
| specified by StartAngle and ends at the angle specified by EndAngle.  
| StartAngle must be less than EndAngle, ie. EndAngle may be greater than 
| 0x10000.  Any portion of the arc that lies outside of the currently defined 
| viewport will not be drawn.
|
| Inputs: Color                - color of arc
|         Center               - center of base ellipse
|         Radius               - radii along major and minor axes of ellipse
|         StartAngle, EndAngle - angles defining sweep of arc
|
| Return: none
*/
extern void	zDrawEllArc(zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius, zangle StartAngle, zangle EndAngle);


/*---------------------------------------------------------------------------
| zDrawEllipse()
|
| Draws an ellipse of the specified color using mode with the center defined 
| by Center.  The radius in the horizontal direction is Raduis->x and the 
| radius in the vertical direction is Radius->y. Any portion of the ellipse 
| that lies outside of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of ellipse
|         Center - center of ellipse
|         Radius - radii along major and minor axes
|
| Return: none
*/
extern void zDrawEllipse(zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius);


/*---------------------------------------------------------------------------
| zDrawFillBox()
|
| Draws a solid rectangle with the specified color using Mode with opposite 
| vertices defined by P1 and P2.  The vertices given do not need to be 
| exactly the top left and bottom right.  They only have to reference 
| opposite vertices of the rectangle.  Any portion of the rectangle that lies
| outside of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of box
|         P1, P2 - diagonal vertices of box
|
| Return: none
*/
extern void zDrawFillBox(zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zDrawFillCircle()
|
| Draws a solid circle of the specified color using Mode with the center 
| located at Center and a radius of Radius.  Any portion of the circle that 
| lies outside of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of circle
|         Center - location of center of circle
|         Radius - radius of circle
|
| Return: none
*/
extern void zDrawFillCircle(zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius);


/*---------------------------------------------------------------------------
| zDrawFillEllipse()
|
| Draws an ellipse of the specified color using Mode with the center defined 
| by Center.  The radius in the horizontal direction is Raduis->x and the 
| radius in the vertical direction is Radius->y.  Any portion of the ellipse 
| that lies outside of the currently defined viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of ellipse
|         Center - location of center of ellipse
|         Radius - radii along major and minor axes
|
| Return: none
*/
extern void zDrawFillEllipse(zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius);


/*---------------------------------------------------------------------------
| zDrawLine()
|
| Draws a line of the specified color using Mode with endpoints located at P1 
| and P2. Any portion of the line that lies outside of the currently defined 
| viewport will not be drawn.
|
| Inputs: Mode   - pixel write mode
|         Color  - color of line
|         P1, P2 - endpoints of line
|
| Return: none
*/
extern void zDrawLine(zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zDrawPoint()
|
| Draws a single point of the specified color using Mode at P.  If the point 
| lies outside of the currently defined viewport, no drawing will take place.
|
| Inputs: Mode  - pixel write mode
|         Color - color of point
|         P     - location of point
|
| Return: none
*/
extern void zDrawPoint(zPixelMode Mode, zbyte Color, z2DPoint *P);


/*---------------------------------------------------------------------------
| zDrawString()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a 
| graphics block similar to the blocks used by zBlockGet() and zBlockPut() 
| and places this block on the screen at the specified coordinates.  If the
| internal buffer is not large enough to hold the block, the function will
| abort.  The characters are drawn with color FgColor using the current font
| and the background is color BgColor.  When Mode zNO_ACTION is used, the
| graphics block is created in memory, retrievable by zGetLastString(), but
| no text is actually drawn on the screen.  (Note: the system font is
| automatically installed when zWhichSVGA() is called.)
|
| Inputs: Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zDrawString(zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zDrawStringDn()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a
| graphics block similar to the blocks used by zBlockGet() and zBlockPut().  
| If the internal buffer is not large enough to hold the block, the function 
| will abort.  The characters are drawn with color FgColor using the current 
| font and the background is color BgColor.  When mode zNO_ACTION is used, 
| the graphics block is created in memory retrievable by zGetLastString(), 
| but no text is actually drawn on the screen.  (Note: the system font is 
| automatically installed when zWhichSVGA() is called.)  The text is drawn 
| upside down with P specifying the lower, right corner.  This corner 
| corresponds to the upper, left corner when the text is oriented upright.
|
| Inputs: Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zDrawStringDn(zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zDrawStringLt()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a 
| graphics block similar to the blocks used by zBlockGet() and zBlockPut().  
| If the internal buffer is not large enough to hold the block, the function 
| will abort.  The characters are drawn with color FgColor using the current 
| font and the background is color BgColor.  When mode zNO_ACTION is used, 
| the graphics block is created in memory retrievable by zGetLastString(), 
| but no text is actually drawn on the screen.  (Note: the system font is 
| automatically installed when zWhichSVGA() is called.)  The text block is 
| rotated 90 degrees to the left with P specifying the lower, left corner.  
| This corner corresponds to the upper, left corner when the text is oriented
| upright.
|
| Inputs: Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zDrawStringLt(zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zDrawStringRt()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a
| graphics block similar to the blocks used by zBlockGet() and zBlockPut().  
| If the internal buffer is not large enough to hold the block, the function 
| will abort.  The characters are drawn with color FgColor using the current 
| font and the background is color BgColor.  When mode zNO_ACTION is used, 
| the graphics block is created in memory retrievable by zGetLastString(), 
| but no text is actually drawn on the screen.  (Note: the system font is 
| automatically installed when zWhichSVGA() is called.)  The text block is 
| rotated 90 degrees to the right with P specifying the upper, right corner.  
| This corner corresponds to the upper, left corner if the text is oriented
| upright.
|
| Inputs: Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zDrawStringRt(zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zFillArea()
|
| Fills a region with a new color specified by FillColor.  The region is 
| defined by any line or curve of the color BorderColor or FillColor or by 
| the edge of the viewport.  All graphics within the region are lost and 
| covered with FillColor.  The border is not effected.
|
| Inputs: SeedPoint   - location where fill begins
|         BorderColor - color defining region boundary
|         FillColor   - color to fill region
|
| Return: none
*/
extern void zFillArea(z2DPoint *SeedPoint, zbyte BorderColor, zbyte FillColor);


/*---------------------------------------------------------------------------
| zFillColor()
|
| Replaces every existence of OldColor with NewColor within a region.  The 
| region is defined as any pixel of OldColor which has a path of pixels of 
| OldColor or NewColor with sides touching back to SeedPoint.  Therefore, 
| only pixels of OldColor are modified and no other information is changed.
|
| Inputs: SeedPoint - location where fill begins
|         OldColor  - color to be replaced
|         NewColor  - color to replace OldColor
|
| Return: none
*/
extern void zFillColor(z2DPoint *SeedPoint, zbyte OldColor, zbyte NewColor);


/*---------------------------------------------------------------------------
| zFillConvexPoly()
|
| Draws a solid polygon outlined by the points given in Array.  The points 
| are expected to be consecutive and the resulting polygon should be convex 
| (ie. the polygon should not curve in on itself like a kidney bean).  The 
| function also assumes that the last point in the array connects to the 
| first.
|
| Inputs: Color     - color of polygon
|         NumPoints - number of vertices in the polygon
|         Array     - points describing the polygon
|
| Return: none
*/
extern void zFillConvexPoly(zbyte Color, zdword NumPoints, z2DPoint *Array);


/*---------------------------------------------------------------------------
| zFillPage()
|
| Clears the currently active page with the specified color.  All information 
| on the page is lost.
|
| Inputs: Color - color to cover page
|
| Return: none
*/
extern void zFillPage(zbyte Color);


/*---------------------------------------------------------------------------
| zFillPoly()
|
| Draws a solid polygon outlined by the points given in Array.  The points 
| are expected to be consecutive.  However, there are no restrictions on the 
| relative locations of the points.  The polygon may curve in on itself or 
| have crossed lines.  The function also assumes that the last point in the 
| array connects to the first.  zFillPoly() can be used interchangeably with 
| zFillConvexPoly(), although zFillPoly() is slightly slower.
|
| Inputs: Color     - color of polygon
|         NumPoints - number of vertices in polygon
|         Array     - points describing the polygon
|
| Return: none
*/
extern void zFillPoly(zbyte Color, zdword NumPoints, z2DPoint *Array);


/*---------------------------------------------------------------------------
| zFillScreen()
|
| Clears the entire screen with the specified color.  All information on the 
| screen is lost.
|
| Inputs: Color - color to cover screen
|
| Return: none
*/
extern void zFillScreen(zbyte Color);


/*---------------------------------------------------------------------------
| zFillView()
|
| Fills the currently defined viewport with the specified color.  All 
| information in the viewport is lost.
|
| Inputs: Color - color to cover viewport
|
| Return:
*/
extern void zFillView(zbyte Color);


/*---------------------------------------------------------------------------
| zFontGetInfo()
|
| Returns the width and height dimensions of the currently loaded font.
|
| Inputs: Width  - receives the width of the current font
|         Height - receives the height of the current font
|
| Return: none
*/
extern void zFontGetInfo(zlong *Width, zlong *Height);


/*---------------------------------------------------------------------------
| zFontSet()
|
| Loads the current font with the data found at Font.  The zFont structure is
| 4098 bytes long with the first two bytes being the width and height of the 
| font respectively.
|
| Inputs: Font - pointer to font data
|
| Return: none
*/
extern void zFontSet(zFont *Font);


/*---------------------------------------------------------------------------
| zFontSystem()
|
| Sets the current font to the standard system font.  This font is 
| automatically set when zInitZSVGA() is called.
|
| Inputs: none
|
| Return: none
*/
extern void zFontSystem(void);


/*---------------------------------------------------------------------------
| zGetArcCos()
|
| Returns the arc cosine for the given value.  The return will be between 0 
| and 0x8000.  CosValue should be between -0x10000 and +0x10000 representing 
| the range -1 to +1.  Input values outside this range are invalid and 
| zGetArcCos() will return 0x80000000.  This function uses an integer lookup 
| table stored within the library in order to produce the fastest possible 
| results.  No floating point arithmetic is used.
|
| Inputs: CosValue - cosine of angle desired
|
| Return: angle for the given cosine value
*/
extern zangle zGetArcCos(z1616fix CosValue);


/*---------------------------------------------------------------------------
| zGetArcSin()
|
| Returns the arc sine for the given value.  The return will be between 
| -0x4000 and +0x4000.  SinValue should be between -0x10000 and +0x10000 
| representing the range -1 to +1.  Input values outside this range are 
| invalid and zGetArcSin() will return 0x80000000.  This function uses an 
| integer lookup table stored within the library in order to produce the 
| fastest possible results.  No floating point arithmetic is used.
|
| Inputs: SinValue - sine of angle desired
|
| Return: angle for the given sine value
*/
extern zangle zGetArcSin(z1616fix SinValue);


/*---------------------------------------------------------------------------
| zGetArcTan()
|
| Returns the arc tangent for the given value.  The return will be between 
| -0x3FFF and +0x3FFF.  Tanvalue can be any value.  This function uses an 
| integer lookup table stored within the library in order to produce the 
| fastest possible results.  No floating point arithmetic is used.
|
| Inputs: TanValue - tangent of angle desired
|
| Return: angle for the given tangent value
*/
extern zangle zGetArcTan(z1616fix TanValue);


/*---------------------------------------------------------------------------
| zGetCos()
|
| Returns the cosine of the specified angle.  Angle can be any value.  This 
| function uses an integer lookup table stored within the library in order to 
| produce the fastest possible results.  No floating point arithmetic is 
| used.
|
| Inputs: Angle - angle for which cosine is desired
|
| Return: cosine of given angle
*/
extern z1616fix zGetCos(zangle Angle);


/*---------------------------------------------------------------------------
| zGetLastString()
|
| Returns a pointer to the zRasterBlock containing the text drawn by the last
| called zDrawString() function.  This block is held in the library's
| internal memory.  This memory is volatile and the block should be copied to
| a new location if it is not going be used one time immediately.  The text
| is oriented in the same manner as the last called zDrawString() function.
| The suggested practice is to call the zDrawString() function using mode 
| zNO_ACTION which does not actually draw text on the screen.
| zGetLastString() should be called immediately after the desired
| zDrawString() function to prevent possible loss of data caused by other
| functions.
|
| Inputs: none
|
| Return: pointer to zRasterBlock
*/
extern zRasterBlock* zGetLastString(void);


/*---------------------------------------------------------------------------
| zGetPoint()
|
| Returns the value of the pixel at location P.  This value is a color index 
| into the current palette.
|
| Inputs: P - location of desired pixel
|
| Return: color of specified point
*/
extern zbyte zGetPoint(z2DPoint *P);


/*---------------------------------------------------------------------------
| zGetSin()
|
| Returns the sine of the specified angle.  Angle can be any value.  This 
| function uses an integer lookup table stored within the library in order to 
| produce the fastest possible results.  No floating point arithmetic is 
| used.
|
| Inputs: Angle - angle for which cosine is desired
|
| Return: cosine of given angle
*/
extern z1616fix zGetSin(zangle Angle);


/*---------------------------------------------------------------------------
| zGetTan()
|
| Returns the tangent of the specified angle.  Angle can be any value except 
| (0x4000+n*0x8000), where n is an integer.  If an invalid angle is given,
| the gettan will return 0x80000000.  This function uses an integer lookup 
| table stored within the library in order to produce the fastest possible 
| results.  No floating point arithmetic is used.
|
| Inputs: Angle - angle for which tangent is desired
|
| Return: tangent of given angle
*/
extern z1616fix zGetTan(zangle Angle);


/*---------------------------------------------------------------------------
| zInitZSVGA()
|
| Initializes the internal library data and receives some vital information 
| about the system.  This function must be called before any other in the 
| library.  A pointer to physical address zero is necessary for the 
| identification process as well as accessing the frame buffer directly.  
| Many of the functions in the library require a small amount of workspace, 
| so a buffer of at least 16384 bytes is required.  The ZSVGA library is
| not responsible for allocating or freeing this buffer.
|
| Inputs: PhysicalZero - pointer to physical address zero
|         Buffer       - pointer to buffer for internal use of library
|         BufferSize   - size in bytes of Buffer
|
| Return: none
*/
extern void zInitZSVGA(void *PhysicalZero, void *Buffer, zdword NumBytes);


/*---------------------------------------------------------------------------
| zJoystickSetTimer()
|
| Sets an internal library variable for the number of iterations the joystick
| polling routine should use.  The joystick routine polls the port Count 
| times.  For initialization purposes a good value to use is 0x4000.  This 
| function should be again with Count equal the maximum value returned by an 
| initialization routine.
|
| Inputs: Count - number of loop interations
|
| Return: none
*/
extern void zJoystickSetTimer(zdword Count);
			   

/*---------------------------------------------------------------------------
| zJoystickGetStatus()
|
| Polls the joystick port and records the values in Status.  Before a program 
| uses the joysticks, it should initialize them by setting the timer to a 
| high value with zJoystickSetTimer() and then, using zJoystickGetStatu(), 
| record the maximum levels of the joysticks.  The timer should be reset with
| the highest level returned so no excess iterations are spent in the polling 
| loop.
|
| Inputs: Status - receives the current status of the joysticks
|
| Return: none
*/
extern void zJoystickGetStatus(zJstkStatus *Status);


/*---------------------------------------------------------------------------
| zPageActive()
|
| Sets the active page as specified by Page.  It works by creating an offset 
| to be added when any bank switching is performed.  This function does not 
| affect the actual display.  A page is defined as the number of 64K banks 
| necessary to contain the current screen resolution.  For example, 640x480 
| requires 307,200 bytes which is a little more than 4.5 64K blocks.  One 
| page in this resolution will be 5 banks.
|
| zPageActive() does not work in 320x200 mode since this mode uses no bank 
| switching.
|
| Inputs: Page - number of page for drawing
|
| Return: one if successful, zero otherwise
*/
extern zlong zPageActive(zlong Page);


/*---------------------------------------------------------------------------
| zPageDisplay()
|
| Tells the video card where in video memory to begin the display.  This 
| display offset is calculated from the specified page and P.  This is done 
| by multiplying P->y times the width of the current resolution, adding P->x 
| and then adding the appropriate number of 64K blocks for page.  P and Page 
| must all be positive numbers, but are otherwise not restricted.  The 
| display will wrap if the values are abnormally large.  Wrapping will happen 
| from side to side as well as from top of memory to bottom of memory.
|
| The display offset will be changed immediately if WaitForSync is set to 
| zFALSE.  Otherwise, the change will not occur until the next vertical sync.
|
| zPageDisplay() does not work in 320x200 mode since this mode uses no bank 
| switching.  In addition, older models of some SVGA cards operating in 
| native mode do not support paging.  In this library these cards are: 
| Acumos, Ahead version A, Everex and Genoa.  To ensure compatibility, a 
| program should make a call similar to:
|
|   z2DPoint P = {0, 0};
|   if ( !zPageDisplay(0, &P, zFALSE) )
|       printf("PAGING NOT SUPPORTED\n");
|
| Note: SVGA cards by ATI Technologies appear to ignore bit 4 (counting from 
| 0 to 19) in its linear addressing mode and assume it is zero.  Therefore, 
| bit 4 of P->x will be ignored by ATI chipsets.
|
| Inputs: Page        - number of page for display
|         P           - coordinate for top left of display
|         WaitForSync - flag to wait for vertical sync
|
| Return: one if successful, zero otherwise
*/
extern zlong zPageDisplay(int Page, z2DPoint *P, zBoolean WaitForSync);


/*---------------------------------------------------------------------------
| zPalChgAuto()
|
| Smoothly fades the colors between FirstColor and LastColor from the palette 
| Pal to NewPal.  The fade will take the specified number of steps with each 
| step occurring on vertical retrace.  zPalChgAuto() does not modify either 
| Pal or NewPal.
|
| Inputs: Pal                   - initial palette
|         NewPal                - final palette
|         FirstColor, LastColor - range of colors to fade
|         NumSteps              - number of steps in fade
|
| Return: none
*/
extern void zPalChgAuto(zPalette *Pal, zPalette *NewPal, zbyte FirstColor, zbyte LastColor, zlong Steps);


/*---------------------------------------------------------------------------
| zPalChgStep()
|
| Changes the palette colors between FirstColor and LastColor by Percent from 
| Pal to NewPal.  This function works very much like zPalChgAuto() except it 
| only takes a single step and returns.  The step taken is a percentage 
| specified by Percent which can range from 0 to 0x10000, 0 to 100 percent.  
| Values beyond this range cause the function to immediately return without 
| making any changes.  Neither pal, nor newpal are modified.
|
| The palette will be changed immediately if WaitForSync is set to zFALSE.  
| Otherwise, the change will not occur until the next vertical sync.
|
| Inputs: Pal                   - initial palette
|         NewPal                - final palette
|         FirstColor, LastColor - range of colors to fade
|         Percent               - percentage of fade from Pal to NewPal
|         WaitForSync           - flag to wait for vertical sync
|
| Return: none
*/
extern void zPalChgStep(zPalette *Pal, zPalette *NewPal, zbyte FirstColor, zbyte LastColor, z1616fix Percent, zBoolean WaitForSync);


/*---------------------------------------------------------------------------
| zPalDimStep()
|
| Fades the specified palette to black by the given percentage between 
| FirstColor and LastColor.  The percentage step from black to Pal is 
| specified by Percent which can range from 0 to 0x10000, 0 to 100 percent.  
| Values beyond this range cause the function to immediately return without 
| making any changes.  Pal is not modified.
|
| The palette will be changed immediately if WaitForSync is set to zFALSE.  
| Otherwise, the change will not occur until the next vertical sync.
|
| Inputs: Pal                   - reference palette
|         FirstColor, LastColor - range of colors to dim
|         Percent               - percentage of fade from color to black
|         WaitForSync           - flag to wait for vertical sync
|
| Return: none
*/
extern void zPalDimStep(zPalette *Pal, zbyte FirstColor, zbyte LastColor, z1616fix Percent, zBoolean WaitForSync);


/*---------------------------------------------------------------------------
| zPalGet()
|
| Returns in Pal the colors from the current palette between FirstColor and 
| LastColor.  Only colors in the specified range are returned and all others 
| in Pal are unchanged.
|
| Inputs: Pal                   - destination of retrieved palette
|         FirstColor, LastColor - range of colors to retrieve
|
| Return: none
*/
extern void zPalGet(zPalette *Pal, zbyte FirstColor, zbyte LastColor);


/*---------------------------------------------------------------------------
| zPalIOAuto()
|
| Smoothly fades the colors between FirstColor and LastColor of Pal to or 
| from solid black.  The number of steps to complete the fade is given by 
| NumSteps.  A negative value corresponds to fading from Pal to black and a 
| positive value fades from black to Pal.  The time required to accomplish 
| the entire fade is equal to NumSteps divided by the vertical refresh rate, 
| usually 60 or 70 Hz.  zPalIOAuto() does not modify Pal.
|
| Inputs: Pal                   - reference palette
|         FirstColor, LastColor - range of colors to fade
|         Step                  - number of steps in fade
|
| Return: none
*/
extern void zPalIOAuto(zPalette *Pal, zbyte FirstColor, zbyte LastColor, zlong Steps);


/*---------------------------------------------------------------------------
| zPalRotate()
|
| shifts the colors in Pal between FirstColor and LastColor by a given number 
| of locations and sets this new palette.  The number of locations the colors 
| are moved is specified by Shift which is between -256 and 256.  Pal is not 
| modified by zPalRotate().
|
| The palette will be changed immediately if WaitForSync is set to zFALSE.  
| Otherwise, the change will not occur until the next vertical sync.
|
| Inputs: Pal                   - palette to rotate
|         FirstColor, LastColor - range of colors to rotate
|         Shift                 - number of positions colors will move
|         WaitForSync           - flag to wait for vertical sync
|
| Return: none
*/
extern void zPalRotate(zPalette *Pal, zbyte FirstColor, zbyte LastColor, zlong Shift, zBoolean WaitForSync);


/*---------------------------------------------------------------------------
| zPalSet()
|
| sets the specified range of colors in the current palette with the 
| corresponding range in the palette Pal.
|
| The palette will be changed immediately if WaitForSync is set to zFALSE.  
| Otherwise, the change will not occur until the next vertical sync.
|
| Inputs: Pal                   - reference palette
|         FirstColor, LastColor - range of colors to set
|         WaitForSync           - flag to wait for vertical sync
|
| Return: none
*/
extern void zPalSet(zPalette *Pal, zbyte FirstColor, zbyte LastColor, zBoolean WaitForSync);


/*---------------------------------------------------------------------------
| zPCXGetInfo()
|
| Reads the file Name and returns the characteristics of the image.  The 
| dimensions of the image are returned in PCXXsize, PCXYsize.  The number of 
| colors in the image's palette is returned in NumColors and the palette is 
| returned in Pal.  The image's palette will begin with the first color in 
| Pal.
|
| PCX version 3.0 is the only specification for 256 color images.  Therefore, 
| only version 3.0 PCX files are accepted.
|
| If zPCXGetInfo() determines that Name is a satisfactory file, the function 
| will return a one.  Otherwise, the function will return one of several 
| error codes as listed below:
| 
|  0 = does not exist or there is some disk I/O problem
| -1 = not a version 3.0 PCX file
| -2 = not run-length-encoded
| -3 = general error
| 
| Inputs: Name               - filename of PCX image file
|         PCXXSize, PCXYSize - receives dimensions of image
|         NumColors          - receives number of colors in image
|         Pal                - receives image palette
|
| Return: one if successful, error code otherwise
*/
extern zlong zPCXGetInfo(const zchar *Name, zdword *PCXXSize, zdword *PCXYSize, zdword *NumColors, zPalette *Pal);


/*---------------------------------------------------------------------------
| zPCXMake()
|
| Takes the bitmap enclosed in the box defined by P1 and P2 and writes a PCX 
| file with the filename specified by Name.  The resulting file uses the PCX 
| version 3.0 specifications.  The points P1 and P2 must be valid coordinates 
| on the currently active page.  Name may specify a full path and drive 
| letter, if necessary.
|
| If zPCXMake() encounters no problems, the function will return a one.
| Otherwise, the function will return one of several error codes concerning 
| Name as listed below:
|
|  0 = disk I/O problem
| -1 = coordinates out of range
|
| Inputs: P1, P2 - locations of upper, left and lower, right corners of block
|         Name   - filename for image file to write
|
| Return: one if successful, error code otherwise
*/
extern zlong zPCXMake(z2DPoint *P1, z2DPoint *P2, const zchar *Name);


/*---------------------------------------------------------------------------
| zPCXPut()
|
| Reads the PCX image from Name and places it on the screen with the top, 
| left corner at P.  The image is automatically clipped to the currently 
| defined viewport.  The image's palette should be obtained by using 
| zPCXGetInfo().
|
| PCX version 3.0 is the only specification for 256 color images.  Therefore, 
| only version 3.0 PCX files are accepted.
|
| If zPCXPut() encounters no problems, the function will return a one.  
| Otherwise, the function will return one of several error codes as listed 
| below:
|
|  0 = does not exist or there is some disk I/O problem
| -1 = not a version 3.0 pcx file
| -2 = not run-length-encoded
| -3 = general error
|
| Inputs: Mode - pixel write mode
|         P    - location to place top left corner
|         Name - filename of PCX image file
|
| Return: one if successful, error code otherwise
*/
extern zlong zPCXPut(zPixelMode Mode, z2DPoint *P, const zchar *Name);


/*---------------------------------------------------------------------------
| zRes320()
|
| Sets video mode 0x13 which is the industry standard 320x200 graphics mode 
| with 256 colors.  This function can be called without calling zWhichSVGA()
| first as this function requires only that a VGA card is present, not a 
| Super VGA card.  If a VGA is not present, the system may crash.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes320(void);


/*---------------------------------------------------------------------------
| zRes640()
|
| Sets the video mode to 640x480 graphics mode with 256 colors.  This 
| function requires that a Super VGA card with at least 512K of video memory 
| be present.  Also, zWhichSVGA() must be called first or the function will 
| fail without changing the video mode.  If the video card does not have 
| sufficient memory, zRes640() will return without changing the video mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes640(void);


/*---------------------------------------------------------------------------
| zRes640l()
|
| Sets the video mode to 640x400 graphics mode with 256 colors.  This 
| function requires that a Super VGA card with at least 256K of video memory 
| be present.  Also, zWhichSVGA() must be called first or the function will 
| fail without changing the video mode.  If the video card does not have 
| sufficient memory, zRes640l() will return without changing the video mode.
|
| Note: The 640x400 is an uncommon resolution.  Many SVGA cards do not 
| support this mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes640l(void);


/*---------------------------------------------------------------------------
| zRes800()
|
| Sets the video mode to 800x600 graphics mode with 256 colors.  This 
| function requires that a Super VGA card with at least 512K of video memory 
| be present.  Also, zWhichSVGA() must be called first or the function will 
| fail without changing the video mode.  If the video card does not have 
| sufficient memory, zRes800() will return without changing the video mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes800(void);


/*---------------------------------------------------------------------------
| zRes1024()
|
| Sets the video mode to 1024x768 graphics mode with 256 colors.  This 
| function requires that a Super VGA card with at least 1 Megabyte of video 
| memory be present.  Also, zWhichSVGA() must be called first or the function 
| will fail without changing the video mode.  If the video card does not have 
| sufficient memory, zRes1024() will return without changing the video mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes1024(void);


/*---------------------------------------------------------------------------
| zRes1280()
|
| Sets the video mode to 1280x1024 graphics mode with 256 colors.  This 
| function requires that a Super VGA card with at least 2 Megabytes of video 
| memory be present.  Also, zWhichSVGA() must be called first or the function 
| will fail without changing the video mode.  If the video card does not have 
| sufficient memory, zRes1280() will return without changing the video mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zRes1280(void);


/*---------------------------------------------------------------------------
| zResText()
|
| Sets video mode three which is the industry standard 80x25 text mode.
|
| Inputs: none
|
| Return: one if successful, zero otherwise
*/
extern zlong zResText(void);


/*---------------------------------------------------------------------------
| zScrollDn()
|
| Shifts the contents of the box described by P1, P2 down by the number of 
| pixels specified by NumRows.  The empty pixels created at the top of the 
| box are filled with Color.  The pixels that are shifted out of the box are 
| lost.  zScrollDn() enforces P2->x >= P1->x and P2->y >= P1->y.  When placed
| within a loop, zScrollDn() will create a scrolling effect.
|
| Inputs: P1, P2  - upper, left and lower, right corners of block
|         NumRows - number of rows to shift block
|         Color   - color to backfill empty rows
|
| Return: none
*/
extern void zScrollDn(z2DPoint *P1, z2DPoint *P2, zdword NumRows, zbyte Color);


/*---------------------------------------------------------------------------
| zScrollLt()
|
| Shifts the contents of the box described by P1, P2 left by the number of 
| pixels specified by NumColumns.  The empty pixels created at the right of 
| the box are filled with Color.  The pixels that are shifted out of the box 
| are lost.  zScrollLt() enforces P2->x >= P1->x and P2->y >= P1->y.  When 
| placed within a loop, zScrollLt() will create a scrolling effect.
|
| Inputs: P1, P2     - upper, left and lower, right corners of block
|         NumColumns - number of columns to shift block
|         Color      - color to backfill empty columns
|
| Return: none
*/
extern void zScrollLt(z2DPoint *P1, z2DPoint *P2, zdword NumColumns, zbyte Color);


/*---------------------------------------------------------------------------
| zScrollRt()
|
| Shifts the contents of the box described by P1, P2 right by the number of 
| pixels specified by NumColumns.  The empty pixels created at the left of 
| the box are filled with Color.  The pixels that are shifted out of the box 
| are lost.  zScrollRt() enforces P2->x >= P1->x and P2->y >= P1->y.  When 
| placed within a loop, zScrollRt() will create a scrolling effect.
|
| Inputs: P1, P2     - upper, left and lower, right corners of block
|         NumColumns - number of columns to shift block
|         Color      - color to backfill empty columns
|
| Return: none
*/
extern void zScrollRt(z2DPoint *P1, z2DPoint *P2, zdword NumColumns, zbyte Color);


/*---------------------------------------------------------------------------
| zScrollUp()
|
| Shifts the contents of the box described by (x1, y1) - (x2, y2) up by the 
| number of pixels specified by NumRows.  The empty pixels created at the 
| bottom of the box are filled with Color.  The pixels that are shifted out 
| of the box are lost.  zScrollUp() enforces P2->x >= P1->x and P2->y >= 
| P1->y.  When placed within a loop, zScrollUp() will create a scrolling
| effect.
|
| Inputs: P1, P2  - upper, left and lower, right corners of block
|         NumRows - number of rows to shift block
|         Color   - color to backfill empty rows
|
| Return: none
*/
extern void zScrollUp(z2DPoint *P1, z2DPoint *P2, zdword NumRows, zbyte Color);


/*---------------------------------------------------------------------------
| zSDelay()
|
| Pauses execution of the program for a period of time specified by Count.  
| This delay remains approximately constant on all machines by using the 
| vertical sync timer of the VGA graphics card which is usually 60 or 70 Hz.
|
| Inputs: Count - number of vertical syncs to wait
|
| Return: none
*/
extern void zSDelay(zdword Count);


/*---------------------------------------------------------------------------
| zSetCard()
|
| zSetCard() sets the card type and installed video memory.  It can be used 
| instead of the zWhichSVGA() function.  However, specifying an incorrect 
| chip type or installed video memory may cause unpredictable results not 
| excluding damage to the SVGA card and/or monitor.  Extreme caution is 
| advised when using this function.  It is recommended that this function 
| only be used when it is suspected that the identification process in 
| zWhichSVGA() failed.  Be absolutely certain that the chip type specified 
| is the actual chip type installed in the computer.
|
| Warning:  Using setcard improperly may cause a system failure or damage.
|
| Inputs: Chip     - code for SVGA card
|         KBMemory - number of kilobytes of memory on card
|
| Return: none
*/
extern void zSetCard(zSVGAChipset Chip, zlong KBMemory);


/*---------------------------------------------------------------------------
| zSetTempBuffer()
|
| Sets the internal library pointer for the temporary buffer.  Many of the 
| functions in the library require a small amount of workspace, so a buffer 
| of at least 16384 bytes is required.  The ZSVGA library is not 
| responsible for allocating or freeing this memory.
|
| Inputs: Buffer     - pointer to buffer for internal use of library
|         BufferSize - size of Buffer in bytes
|
| Return: none
*/
extern void zSetTempBuffer(void *Buffer, zdword BufferSize);


/*---------------------------------------------------------------------------
| zSetVESA()
|
| zSetVESA() sets the ZSVGA library to use the VESA standard to access the
| video card.  The VESAInfo block is divided into six 256 byte chunks.	The
| first 256 bytes contain information concerning general VESA support for the
| card.  The remaining 5 chunks give information specific to the five resolu-
| tions supported: 640x400, 640x480, 800x600, 1024x768, and 1280x1024.	The
| zWhichVESA() function provided in zwhvesa.c will fill in the entire block
| automatically.
|
| Inputs: VESAInfo - pointer to a 1536 byte block containing VESA information
|
| Return: VESA version number if successful, zero otherwise
*/
extern zdword zSetVESA(void *VESAInfo);


/*---------------------------------------------------------------------------
| zSetView()
|
| zSetView() defines a viewport for clipping output on the screen.  Nothing
| can be drawn outside of the currently defined viewport.  The zRes functions 
| set the viewport to the full screen.  The restrictions on P1 and P2 are as 
| follows:
|
| 0 <= P1->x < P2->x <= zMaxX
| 0 <= P1->y < P2->y <= (32767, effectively unbounded)
|
| Inputs: P1, P2 - locations of upper, left and lower, right corners of block
|
| Return: none
*/
extern void zSetView(z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zSpriteCollDetect()
|
| Reports the collision status between two sprites in sprite graphics or 
| animation.  Sprite1 contains the first sprite which should have been 
| previously defined by zBlockGet() or similar function.  Sprite2, likewise, 
| contains the second sprite.  The top, left corner of the first and second 
| sprites' locations are specified by P1 and P2, respectively.
| 
| zSpriteCollDetect() will return a zero if the sprites are not colliding and 
| the blocks are not overlapping.  The return value will be one if the blocks 
| overlap, but the sprites are not colliding.  A return value of two 
| indicates that the sprites have at least one overlapping pixel and are, 
| therefore, colliding.
|
| Inputs: TransColor       - color representing transparency in sprites
|         P1, P2           - location of top left corner of sprites
|         Sprite1, Sprite2 - sprites to compare
|
| Return: two if sprites are colliding, one if blocks are overlapping, zero 
|         otherwise
*/
extern zlong zSpriteCollDetect(zbyte TransColor, z2DPoint *P1, z2DPoint *P2, zRasterBlock *Sprite1, zRasterBlock *Sprite2);


/*---------------------------------------------------------------------------
| zSpriteGAP()
|
| ("Sprite-Get-And-Put") Used in sprite graphics or animation to retrieve a 
| sprite's background and then display the sprite.  Sprite contains the 
| sprite which should have been previously defined by zBlockGet() or similar 
| function.  TransColor is the transparent color assumed in the sprite.  
| SpriteBkgnd, a pointer to a zRasterBlock structure the same size as the 
| sprite, will receive the sprite's background.  The top, left corner of the 
| sprite's location is specified by P.
|
| Inputs: TransColor  - color representing transparency in sprites
|         P           - location of top left corner
|         Sprite      - the sprite to draw
|         SpriteBkgnd - buffer to store the data below the sprite
|
| Return: none
*/
extern void zSpriteGAP(zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite, zRasterBlock *SpriteBkgnd);


/*---------------------------------------------------------------------------
| zSpriteGet()
|
| Used in sprite graphics or animation to retrieve the background for a 
| sprite, normally just before using zSpritePut().  Sprite contains the 
| sprite which should have been previously defined by zBlockGet() or similar 
| function.  TransColor is the transparent color assumed in the sprite.  
| Spritebkgnd, a pointer to a zRasterBlock structure the same size as the 
| sprite, will receive the sprite's background.  The top, left corner of the 
| sprite's location is specified by P.
|
| Inputs: TransColor  - color representing transparency in sprites
|         P           - location of top left corner
|         Sprite      - reference sprite
|         SpriteBkgnd - buffer to store the data below the sprite
|
| Return: none
*/
extern void zSpriteGet(zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite, zRasterBlock *SpriteBkgnd);


/*---------------------------------------------------------------------------
| zSpritePut()
|
| Used in sprite graphics or animation to display a sprite or, more commonly, 
| its background.  Sprite contains the sprite which should have been 
| previously defined by zBlockGet(),  zSpriteGAP() or zSpriteGet().  
| TransColor is the transparent color assumed in Sprite.  The top, left 
| corner of the sprite's location is specified by P.
|
| Inputs: TransColor - color representing transparency in sprite
|         P          - location of top left corner
|         Sprite     - the sprite to draw
|
| Return: none
*/
extern void zSpritePut(zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite);


/*---------------------------------------------------------------------------
| zVideoModeGet()
|
| Returns the current video mode.  This function is best used to retrieve the 
| video mode being used when a program begins.  When the program ends, this 
| video mode can then be restored using zVideoModeSet().
|
| Inputs: none
|
| Return: current video mode number
*/
extern zlong zVideoModeGet(void);


/*---------------------------------------------------------------------------
| zVideoModeSet()
|
| Sets the video mode specified by mode.  This function is best used at the 
| end of a program to restore the video mode to the mode in use when the 
| program began.  The program should retrieve the video mode at the beginning 
| by using zVideoModeGet().
|
| Inputs: VideoMode - desired video mode number
|
| Return: none
*/
extern void zVideoModeSet(zlong VideoMode);


/*---------------------------------------------------------------------------
| zVideoOff()
|
| Turns off the display output.  Graphics may still be drawn to the screen.  
| However, the computer's monitor will display nothing and appear black.  
| This function can be used to hide graphics being drawn by initially using 
| zVideoOff() and then later calling zVideoOn().
|
| Inputs: none
|
| Return: none
*/
extern void zVideoOff(void);


/*---------------------------------------------------------------------------
| zVideoOn()
|
| Turns the display on.  All graphics that were drawn while the display was 
| off are now visible.  This function can be used to hide graphics being 
| drawn by initially using zVideoOff and then later calling zVideoOn().
|
| Inputs: none
|
| Return: none
*/
extern void zVideoOn(void);


/*---------------------------------------------------------------------------
| zWhichJoystick()
|
| Returns the joystick support available on the computer.  This function 
| should be called prior to use of the joysticks to verify that joysticks are 
| available.  If the function returns a 0, there is no joystick port present.  
| Joystick A is bit 1 and B is bit 2.  Therefore, a return value of 1 means 
| joystick A is available, a value of 2 means B is available and a value of 3 
| means both are available.
|
| Inputs: none
|
| Return: available joystick support
*/
extern zlong zWhichJoystick(void);


/*---------------------------------------------------------------------------
| zWhichMem()
|
| Returns the amount of installed video memory as previously determined by 
| zWhichSVGA().  zWhichSVGA() should be called prior to zWhichMem().  This 
| function should be called prior to any of the zRes functions to verify that 
| there is enough memory to support the desired resolution.  If zSetCard() 
| was used to set the video card and memory, zWhichMem() will return the 
| amount of memory as defined by zSetCard().
|
| Inputs: none
|
| Return: kilobytes of video memory
*/
extern zlong zWhichMem(void);


/*---------------------------------------------------------------------------
| zWhichSVGA()
|
| Identifies the video card installed and the amount of video memory.  The 
| code returned identifies the video card.
|
| No graphics functions should be called unless the video card is properly 
| identified.
|
| Inputs: none
|
| Return: code identifying the installed video card, zero otherwise
*/
extern zSVGAChipset zWhichSVGA(void);


#ifdef __cplusplus
}
#endif


/*---------------------------------------------------------------------------
| By default Watcom does not prefix an underscore or pass arguments on the
| stack and expects functions to remove their own arguments from the stack.
| These pragmas take care of this.
*/
#ifdef __WATCOMC__
#pragma aux ZSVGA_C "_*" parm caller [];
/* Global Variables */
#pragma aux (ZSVGA_C) zMaxX;
#pragma aux (ZSVGA_C) zMaxY;
#pragma aux (ZSVGA_C) zViewX1;
#pragma aux (ZSVGA_C) zViewY1;
#pragma aux (ZSVGA_C) zViewX2;
#pragma aux (ZSVGA_C) zViewY2;
/* Global Functions */
#pragma aux (ZSVGA_C) zBlockGet;
#pragma aux (ZSVGA_C) zBlockPut;
#pragma aux (ZSVGA_C) zBlockResize;
#pragma aux (ZSVGA_C) zBlockRotate;
#pragma aux (ZSVGA_C) zBlockRotateSize;
#pragma aux (ZSVGA_C) zByteCopy;
#pragma aux (ZSVGA_C) z2DRotate;
#pragma aux (ZSVGA_C) z2DScale;
#pragma aux (ZSVGA_C) z2DTranslate;
#pragma aux (ZSVGA_C) z3DProject;
#pragma aux (ZSVGA_C) z3DRotate;
#pragma aux (ZSVGA_C) z3DScale;
#pragma aux (ZSVGA_C) z3DTranslate;
#pragma aux (ZSVGA_C) zDrawAALine;
#pragma aux (ZSVGA_C) zDrawBox;
#pragma aux (ZSVGA_C) zDrawCirArc;
#pragma aux (ZSVGA_C) zDrawCircle;
#pragma aux (ZSVGA_C) zDrawCubicBezier;
#pragma aux (ZSVGA_C) zDrawEllArc;
#pragma aux (ZSVGA_C) zDrawEllipse;
#pragma aux (ZSVGA_C) zDrawFillBox;
#pragma aux (ZSVGA_C) zDrawFillCircle;
#pragma aux (ZSVGA_C) zDrawFillEllipse;
#pragma aux (ZSVGA_C) zDrawLine;
#pragma aux (ZSVGA_C) zDrawPoint;
#pragma aux (ZSVGA_C) zDrawString;
#pragma aux (ZSVGA_C) zDrawStringDn;
#pragma aux (ZSVGA_C) zDrawStringLt;
#pragma aux (ZSVGA_C) zDrawStringRt;
#pragma aux (ZSVGA_C) zFillArea;
#pragma aux (ZSVGA_C) zFillColor;
#pragma aux (ZSVGA_C) zFillConvexPoly;
#pragma aux (ZSVGA_C) zFillPage;
#pragma aux (ZSVGA_C) zFillPoly;
#pragma aux (ZSVGA_C) zFillScreen;
#pragma aux (ZSVGA_C) zFillView;
#pragma aux (ZSVGA_C) zFontGetInfo;
#pragma aux (ZSVGA_C) zFontSet;
#pragma aux (ZSVGA_C) zFontSystem;
#pragma aux (ZSVGA_C) zGetLastString;
#pragma aux (ZSVGA_C) zGetArcCos;
#pragma aux (ZSVGA_C) zGetArcSin;
#pragma aux (ZSVGA_C) zGetArcTan;
#pragma aux (ZSVGA_C) zGetCos;
#pragma aux (ZSVGA_C) zGetPoint;
#pragma aux (ZSVGA_C) zGetSin;
#pragma aux (ZSVGA_C) zGetTan;
#pragma aux (ZSVGA_C) zInitZSVGA;
#pragma aux (ZSVGA_C) zJoystickSetTimer;
#pragma aux (ZSVGA_C) zJoystickGetStatus;
#pragma aux (ZSVGA_C) zPageActive;
#pragma aux (ZSVGA_C) zPageDisplay;
#pragma aux (ZSVGA_C) zPalChgAuto;
#pragma aux (ZSVGA_C) zPalChgStep;
#pragma aux (ZSVGA_C) zPalDimStep;
#pragma aux (ZSVGA_C) zPalGet;
#pragma aux (ZSVGA_C) zPalIOAuto;
#pragma aux (ZSVGA_C) zPalRotate;
#pragma aux (ZSVGA_C) zPalSet;
#pragma aux (ZSVGA_C) zPCXGetInfo;
#pragma aux (ZSVGA_C) zPCXMake;
#pragma aux (ZSVGA_C) zPCXPut;
#pragma aux (ZSVGA_C) zRes320;
#pragma aux (ZSVGA_C) zRes640;
#pragma aux (ZSVGA_C) zRes640l;
#pragma aux (ZSVGA_C) zRes800;
#pragma aux (ZSVGA_C) zRes1024;
#pragma aux (ZSVGA_C) zRes1280;
#pragma aux (ZSVGA_C) zResText;
#pragma aux (ZSVGA_C) zScrollDn;
#pragma aux (ZSVGA_C) zScrollLt;
#pragma aux (ZSVGA_C) zScrollRt;
#pragma aux (ZSVGA_C) zScrollUp;
#pragma aux (ZSVGA_C) zSDelay;
#pragma aux (ZSVGA_C) zSetCard;
#pragma aux (ZSVGA_C) zSetTempBuffer;
#pragma aux (ZSVGA_C) zSetVESA;
#pragma aux (ZSVGA_C) zSetView;
#pragma aux (ZSVGA_C) zSpriteCollDetect;
#pragma aux (ZSVGA_C) zSpriteGAP;
#pragma aux (ZSVGA_C) zSpriteGet;
#pragma aux (ZSVGA_C) zSpritePut;
#pragma aux (ZSVGA_C) zVideoModeGet;
#pragma aux (ZSVGA_C) zVideoModeSet;
#pragma aux (ZSVGA_C) zVideoOff;
#pragma aux (ZSVGA_C) zVideoOn;
#pragma aux (ZSVGA_C) zWhichJoystick;
#pragma aux (ZSVGA_C) zWhichMem;
#pragma aux (ZSVGA_C) zWhichSVGA;
#endif


/*=========================================================================*/
#endif /* ZSVGA_H */
