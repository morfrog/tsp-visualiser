/***************************************************************************/
/*                                                                         */
/*                                ZVSVGA.H                                 */
/*                                                                         */
/***************************************************************************/
/*      (c) 1995 Zephyr Software, Stephen L. Balkum and Daniel A. Sill     */
/***************************************************************************/
/* The content of this file is confidential.  It should not be released to */
/* the general public without proper consent.                              */
/***************************************************************************/

/*--------------------------------------------------------------------------\
| This file contains the function prototypes and structure definitions for
| the Zephyr Software protected mode, virtual screen graphics library.
|
| Revision History:
|
| Aug 15, 1995: First pass complete
| Sep 01, 1995: Updated documentation for missing v's, Added zvBlit()
| Sep 23, 1995: Watcom pragmas now take care of stack passing
| Dec 18, 1995: Added zvBlit() to Watcom pragma list.
\--------------------------------------------------------------------------*/


#ifndef ZVSVGA_H
#define ZVSVGA_H


#include "ztypes.h"


/*---------------------------------------------------------------------------
| Describes the characteristics of a system memory buffer to be used as a 
| virtual screen.
*/
typedef struct tag_zVirtualScreen
{
    zbyte* Buffer;           /* pointer to memory buffer        */
    zdword FullWidth;        /* bytes per scanline              */
    zdword FullHeight;       /* total number of rows            */
    zdword ViewX1;           /* define the current clipping box */
    zdword ViewY1;
    zdword ViewX2;
    zdword ViewY2;
} zVirtualScreen;


#ifdef __cplusplus
extern "C" {
#endif


/*=========================================================================*\
| Function Prototypes                                                       |
\*=========================================================================*/


/*---------------------------------------------------------------------------
| zvBlit()
|
| Copies a block from one screen to another.  The source and destination can
| each be a zVirtualScreen or the video card.  The video card is specified
| by a NULL value and the source and destination cannot both refer to the
| video card.  The coordinates P0 and P1 must both be valid on the source
| screen, but there are no restrictions for P2.
|
| Inputs: SrcScreen  - pointer to descriptor of source screen
|         P0, P1     - locations of upper, left and lower, right corners
|         DestScreen - pointer to descriptor of destination screen
|         P2         - location of upper, left corner
|
| Return: one for success, zero otherwise
*/
extern zlong zvBlit(zVirtualScreen *SrcScreen, z2DPoint *P0, z2DPoint *P1, zVirtualScreen *DestScreen, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvBlockGet()
|
| Stores the pixel data contained within the block defined by P1-P2 in the
| buffer referenced by GfxBlk.  Memory for GfxBlk must be allocated as a
| zRasterBlock structure with a size in bytes equal to the pixel area of the
| block plus eight bytes for the width and height integers of the
| zRasterBlock structure.
|
| Note, however, that GfxBlk can be quite large.  If the size of GfxBlk is
| insufficient, zvBlockGet() will overwrite any data in memory contained
| beyond GfxBlk and may possibly cause the system to crash. zvBlockGet()
| enforces (P2->x) >= (P1->x) and (P2->y) >= (P1->y).  Also, the coordinates
| must be valid on the screen at the current resolution.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P1, P2  - locations of upper, left and lower, right corners
|         GfxBlk  - system memory to receive the image block
|
| Return: one for success, zero if the coordinates are invalid
*/
extern zlong zvBlockGet(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, zRasterBlock *GfxBlk);


/*---------------------------------------------------------------------------
| zvBlockPut()
|
| Places the pixel data contained in the variable referenced by GfxBlk on the
| screen.  The top, left corner of the block is specified by P.  Any (P->x,
| P->y) is acceptable and any portion of the block that lies outside of the 
| currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         P       - location on the active screen of the upper, left corner
|         GfxBlk  - system memory holding the image block
|
| Return: none
*/
extern void zvBlockPut(zVirtualScreen *VScreen, zPixelMode Mode, z2DPoint *P, zRasterBlock *GfxBlk);


/*---------------------------------------------------------------------------
| zvDrawAALine()
|
| Draws an antialiased line of the specified color with endpoints located at 
| P1 and P2.  Antialiased lines trick the eye into seeing true vector lines 
| instead of a jagged series of individual pixels.  This is accomplished by 
| drawing two pixels of the same color but with different intensities 
| (brightness) for each pixel drawn by zvDrawAALine().  The eye averages the 
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
| Inputs: VScreen      - pointer to descriptor of virtual screen
|         NumColorBits - power-of-two shades of Color available
|         Color        - base color of line
|         P1, P2       - endpoints of line
|
| Return: none
*/
extern void zvDrawAALine(zVirtualScreen *VScreen, zlong NumColorBits, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvDrawBox()
|
| Draws a rectangle with the specified color using Mode with opposite 
| vertices defined by P1 and P2.  The vertices given do not need to be 
| exactly the top left and bottom right.  They only have to reference 
| opposite vertices of the rectangle.  Any portion of the rectangle that lies
| outside of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of box
|         P1, P2  - diagonal corners of box
|
| Return: none
*/
extern void zvDrawBox(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvDrawCirArc()
|
| Draws a circular arc of the specified color and mode with the center 
| located at Center, radius of Radius, starting at the angle specified by 
| StartAngle and ending at the angle specified by EndAngle.  However, 
| StartAngle must be less than EndAngle, ie. EndAngle may be larger than 
| 0x10000.  Any portion of the arc that lies outside of the currently defined
| viewport will not be drawn.
|
| Inputs: VScreen              - pointer to descriptor of virtual screen
|         Mode                 - pixel write mode
|         Color                - color of arc
|         Center               - location of center of base circle
|         Radius               - radius of base circle
|         StartAngle, EndAngle - angles defining sweep of arc
|
| Return: none
*/
extern void zvDrawCirArc(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius, zangle StartAngle, zangle EndAngle);


/*---------------------------------------------------------------------------
| zvDrawCircle()
|
| Draws a circle of the specified color using Mode with the center located at 
| Center and a radius of Radius.  Any portion of the circle that lies outside 
| of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of circle
|         Center  - location of center of circle
|         Radius  - radius of circle
|
| Return: none
*/
extern void zvDrawCircle(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius);


/*---------------------------------------------------------------------------
| zvDrawCubicBezier()
|
| Draws a standard cubic Bezier curve connecting the points POn1 and POn2 
| using the specified mode and color.  The off-curve control points are POff1 
| and POff2.  A technical description of cubic Bezier curves is beyond the 
| scope of this manual.  However, most advanced computer graphics texts 
| contain discussions on this topic.
|
| Inputs: VScreen                  - pointer to descriptor of virtual screen
|         Mode                     - pixel write mode
|         Color                    - color of curve
|         POn1, POff1, POff2, POn2 - the control points defining the curve
|
| Return: none
*/
extern void zvDrawCubicBezier(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *POn1, z2DPoint *POff1, z2DPoint *POff2, z2DPoint *POn2);


/*---------------------------------------------------------------------------
| zvDrawEllArc()
|
| Draws an elliptical arc of the specified color and mode with the center 
| located at Center.  The radius in the horizontal direction is Raduis->x and
| the radius in the vertical direction is Radius->y.  The starting angle is 
| specified by StartAngle and ends at the angle specified by EndAngle.  
| StartAngle must be less than EndAngle, ie. EndAngle may be greater than 
| 0x10000.  Any portion of the arc that lies outside of the currently defined 
| viewport will not be drawn.
|
| Inputs: VScreen              - pointer to descriptor of virtual screen
|         Color                - color of arc
|         Center               - center of base ellipse
|         Radius               - radii along major and minor axes of ellipse
|         StartAngle, EndAngle - angles defining sweep of arc
|
| Return: none
*/
extern void	zvDrawEllArc(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius, zangle StartAngle, zangle EndAngle);


/*---------------------------------------------------------------------------
| zvDrawEllipse()
|
| Draws an ellipse of the specified color using mode with the center defined 
| by Center.  The radius in the horizontal direction is Raduis->x and the 
| radius in the vertical direction is Radius->y. Any portion of the ellipse 
| that lies outside of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of ellipse
|         Center  - center of ellipse
|         Radius  - radii along major and minor axes
|
| Return: none
*/
extern void zvDrawEllipse(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius);


/*---------------------------------------------------------------------------
| zvDrawFillBox()
|
| Draws a solid rectangle with the specified color using Mode with opposite 
| vertices defined by P1 and P2.  The vertices given do not need to be 
| exactly the top left and bottom right.  They only have to reference 
| opposite vertices of the rectangle.  Any portion of the rectangle that lies
| outside of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of box
|         P1, P2  - diagonal vertices of box
|
| Return: none
*/
extern void zvDrawFillBox(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvDrawFillCircle()
|
| Draws a solid circle of the specified color using Mode with the center 
| located at Center and a radius of Radius.  Any portion of the circle that 
| lies outside of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of circle
|         Center  - location of center of circle
|         Radius  - radius of circle
|
| Return: none
*/
extern void zvDrawFillCircle(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, zlong Radius);


/*---------------------------------------------------------------------------
| zvDrawFillEllipse()
|
| Draws an ellipse of the specified color using Mode with the center defined 
| by Center.  The radius in the horizontal direction is Raduis->x and the 
| radius in the vertical direction is Radius->y.  Any portion of the ellipse 
| that lies outside of the currently defined viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of ellipse
|         Center  - location of center of ellipse
|         Radius  - radii along major and minor axes
|
| Return: none
*/
extern void zvDrawFillEllipse(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *Center, z2DPoint *Radius);


/*---------------------------------------------------------------------------
| zvDrawLine()
|
| Draws a line of the specified color using Mode with endpoints located at P1 
| and P2. Any portion of the line that lies outside of the currently defined 
| viewport will not be drawn.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of line
|         P1, P2  - endpoints of line
|
| Return: none
*/
extern void zvDrawLine(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvDrawPoint()
|
| Draws a single point of the specified color using Mode at P.  If the point 
| lies outside of the currently defined viewport, no drawing will take place.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         Color   - color of point
|         P       - location of point
|
| Return: none
*/
extern void zvDrawPoint(zVirtualScreen *VScreen, zPixelMode Mode, zbyte Color, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvDrawString()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a 
| graphics block similar to the blocks used by zvBlockGet() and zvBlockPut() 
| and places this block on the screen at the specified coordinates.  If the
| internal buffer is not large enough to hold the block, the function will
| abort.  The characters are drawn with color FgColor using the current font
| and the background is color BgColor.  When Mode zNO_ACTION is used, the
| graphics block is created in memory, retrievable by zGetLastString(), but
| no text is actually drawn on the screen.  (Note: the system font is
| automatically installed when zWhichSVGA() is called.)
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zvDrawString(zVirtualScreen *VScreen, zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvDrawStringDn()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a
| graphics block similar to the blocks used by zvBlockGet() and zvBlockPut().  
| If the internal buffer is not large enough to hold the block, the function 
| will abort.  The characters are drawn with color FgColor using the current 
| font and the background is color BgColor.  When mode zNO_ACTION is used, 
| the graphics block is created in memory retrievable by zGetLastString(), 
| but no text is actually drawn on the screen.  (Note: the system font is 
| automatically installed when zWhichSVGA() is called.)  The text is drawn 
| upside down with P specifying the lower, right corner.  This corner 
| corresponds to the upper, left corner when the text is oriented upright.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zvDrawStringDn(zVirtualScreen *VScreen, zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvDrawStringLt()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a 
| graphics block similar to the blocks used by zvBlockGet() and zvBlockPut().  
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
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zvDrawStringLt(zVirtualScreen *VScreen, zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvDrawStringRt()
|
| Takes the null terminated ASCII characters beginning at Strng and creates a
| graphics block similar to the blocks used by zvBlockGet() and zvBlockPut().  
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
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         FgColor - color of text
|         BgColog - color of background
|         Strng   - pointer to null terminated string
|         P       - location of upper left corner of string
|
| Return: none
*/
extern void zvDrawStringRt(zVirtualScreen *VScreen, zPixelMode Mode, zbyte FgColor, zbyte BgColor, const zchar *Strng, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvFillArea()
|
| Fills a region with a new color specified by FillColor.  The region is 
| defined by any line or curve of the color BorderColor or FillColor or by 
| the edge of the viewport.  All graphics within the region are lost and 
| covered with FillColor.  The border is not effected.
|
| Inputs: VScreen     - pointer to descriptor of virtual screen
|         SeedPoint   - location where fill begins
|         BorderColor - color defining region boundary
|         FillColor   - color to fill region
|
| Return: none
*/
extern void zvFillArea(zVirtualScreen *VScreen, z2DPoint *SeedPoint, zbyte BorderColor, zbyte FillColor);


/*---------------------------------------------------------------------------
| zvFillColor()
|
| Replaces every existence of OldColor with NewColor within a region.  The 
| region is defined as any pixel of OldColor which has a path of pixels of 
| OldColor or NewColor with sides touching back to SeedPoint.  Therefore, 
| only pixels of OldColor are modified and no other information is changed.
|
| Inputs: VScreen   - pointer to descriptor of virtual screen
|         SeedPoint - location where fill begins
|         OldColor  - color to be replaced
|         NewColor  - color to replace OldColor
|
| Return: none
*/
extern void zvFillColor(zVirtualScreen *VScreen, z2DPoint *SeedPoint, zbyte OldColor, zbyte NewColor);


/*---------------------------------------------------------------------------
| zvFillConvexPoly()
|
| Draws a solid polygon outlined by the points given in Array.  The points 
| are expected to be consecutive and the resulting polygon should be convex 
| (ie. the polygon should not curve in on itself like a kidney bean).  The 
| function also assumes that the last point in the array connects to the 
| first.
|
| Inputs: VScreen   - pointer to descriptor of virtual screen
|         Color     - color of polygon
|         NumPoints - number of vertices in the polygon
|         Array     - points describing the polygon
|
| Return: none
*/
extern void zvFillConvexPoly(zVirtualScreen *VScreen, zbyte Color, zdword NumPoints, z2DPoint *Array);


/*---------------------------------------------------------------------------
| zvFillPoly()
|
| Draws a solid polygon outlined by the points given in Array.  The points 
| are expected to be consecutive.  However, there are no restrictions on the 
| relative locations of the points.  The polygon may curve in on itself or 
| have crossed lines.  The function also assumes that the last point in the 
| array connects to the first.  zvFillPoly() can be used interchangeably with
| zvFillConvexPoly(), although zvFillPoly() is slightly slower.
|
| Inputs: VScreen   - pointer to descriptor of virtual screen
|         Color     - color of polygon
|         NumPoints - number of vertices in polygon
|         Array     - points describing the polygon
|
| Return: none
*/
extern void zvFillPoly(zVirtualScreen *VScreen, zbyte Color, zdword NumPoints, z2DPoint *Array);


/*---------------------------------------------------------------------------
| zvFillScreen()
|
| Clears the entire screen with the specified color.  All information on the 
| screen is lost.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Color   - color to cover screen
|
| Return: none
*/
extern void zvFillScreen(zVirtualScreen *VScreen, zbyte Color);


/*---------------------------------------------------------------------------
| zvFillView()
|
| Fills the currently defined viewport with the specified color.  All 
| information in the viewport is lost.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Color   - color to cover viewport
|
| Return:
*/
extern void zvFillView(zVirtualScreen *VScreen, zbyte Color);


/*---------------------------------------------------------------------------
| zvGetPoint()
|
| Returns the value of the pixel at location P.  This value is a color index 
| into the current palette.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P       - location of desired pixel
|
| Return: color of specified point
*/
extern zbyte zvGetPoint(zVirtualScreen *VScreen, z2DPoint *P);


/*---------------------------------------------------------------------------
| zvPCXMake()
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
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P1, P2  - locations of upper, left and lower, right corners of block
|         Name    - filename for image file to write
|
| Return: one if successful, error code otherwise
*/
extern zlong zvPCXMake(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, const zchar *Name);


/*---------------------------------------------------------------------------
| zvPCXPut()
|
| Reads the PCX image from Name and places it on the screen with the top, 
| left corner at P.  The image is automatically clipped to the currently 
| defined viewport.  The image's palette should be obtained by using 
| zPCXGetInfo().
|
| PCX version 3.0 is the only specification for 256 color images.  Therefore, 
| only version 3.0 PCX files are accepted.
|
| If zvPCXPut() encounters no problems, the function will return a one.  
| Otherwise, the function will return one of several error codes as listed 
| below:
|
|  0 = does not exist or there is some disk I/O problem
| -1 = not a version 3.0 pcx file
| -2 = not run-length-encoded
| -3 = general error
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         Mode    - pixel write mode
|         P       - location to place top left corner
|         Name    - filename of PCX image file
|
| Return: one if successful, error code otherwise
*/
extern zlong zvPCXPut(zVirtualScreen *VScreen, zPixelMode Mode, z2DPoint *P, const zchar *Name);


/*---------------------------------------------------------------------------
| zvScrollDn()
|
| Shifts the contents of the box described by P1, P2 down by the number of 
| pixels specified by NumRows.  The empty pixels created at the top of the 
| box are filled with Color.  The pixels that are shifted out of the box are 
| lost.  zvScrollDn() enforces P2->x * P1->x and P2->y * P1->y.  When placed 
| within a loop, zvScrollDn() will create a scrolling effect.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P1, P2  - upper, left and lower, right corners of block
|         NumRows - number of rows to shift block
|         Color   - color to backfill empty rows
|
| Return: none
*/
extern void zvScrollDn(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, zdword NumRows, zbyte Color);


/*---------------------------------------------------------------------------
| zvScrollLt()
|
| Shifts the contents of the box described by P1, P2 left by the number of 
| pixels specified by NumColumns.  The empty pixels created at the right of 
| the box are filled with Color.  The pixels that are shifted out of the box 
| are lost.  zvScrollLt() enforces P2->x * P1->x and P2->y * P1->y.  When 
| placed within a loop, zvScrollLt() will create a scrolling effect.
|
| Inputs: VScreen    - pointer to descriptor of virtual screen
|         P1, P2     - upper, left and lower, right corners of block
|         NumColumns - number of columns to shift block
|         Color      - color to backfill empty columns
|
| Return: none
*/
extern void zvScrollLt(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, zdword NumColumns, zbyte Color);


/*---------------------------------------------------------------------------
| zvScrollRt()
|
| Shifts the contents of the box described by P1, P2 right by the number of 
| pixels specified by NumColumns.  The empty pixels created at the left of 
| the box are filled with Color.  The pixels that are shifted out of the box 
| are lost.  zvScrollRt() enforces P2->x * P1->x and P2->y * P1->y.  When 
| placed within a loop, zvScrollRt() will create a scrolling effect.
|
| Inputs: VScreen    - pointer to descriptor of virtual screen
|         P1, P2     - upper, left and lower, right corners of block
|         NumColumns - number of columns to shift block
|         Color      - color to backfill empty columns
|
| Return: none
*/
extern void zvScrollRt(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, zdword NumColumns, zbyte Color);


/*---------------------------------------------------------------------------
| zvScrollUp()
|
| Shifts the contents of the box described by (x1, y1) - (x2, y2) up by the 
| number of pixels specified by NumRows.  The empty pixels created at the 
| bottom of the box are filled with Color.  The pixels that are shifted out 
| of the box are lost.  zvScrollUp() enforces P2->x * P1->x and
| P2->y * P1->y.  When placed within a loop, zvScrollUp() will create a
| scrolling effect.
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P1, P2  - upper, left and lower, right corners of block
|         NumRows - number of rows to shift block
|         Color   - color to backfill empty rows
|
| Return: none
*/
extern void zvScrollUp(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2, zdword NumRows, zbyte Color);


/*---------------------------------------------------------------------------
| zvSetView()
|
| zvSetView() defines a viewport for clipping output on the screen.  Nothing
| can be drawn outside of the currently defined viewport.  The restrictions
| on P1 and P2 are as follows:
|
| 0 <= P1->x < P2->x <= zMaxX
| 0 <= P1->y < P2->y <= (32767, effectively unbounded)
|
| Inputs: VScreen - pointer to descriptor of virtual screen
|         P1, P2  - locations of upper, left and lower, right corners of block
|
| Return: none
*/
extern void zvSetView(zVirtualScreen *VScreen, z2DPoint *P1, z2DPoint *P2);


/*---------------------------------------------------------------------------
| zvSpriteGAP()
|
| ("Sprite-Get-And-Put") Used in sprite graphics or animation to retrieve a 
| sprite's background and then display the sprite.  Sprite contains the 
| sprite which should have been previously defined by zvBlockGet() or similar
| function.  TransColor is the transparent color assumed in the sprite.  
| SpriteBkgnd, a pointer to a zRasterBlock structure the same size as the 
| sprite, will receive the sprite's background.  The top, left corner of the 
| sprite's location is specified by P.
|
| Inputs: VScreen     - pointer to descriptor of virtual screen
|         TransColor  - color representing transparency in sprites
|         P           - location of top left corner
|         Sprite      - the sprite to draw
|         SpriteBkgnd - buffer to store the data below the sprite
|
| Return: none
*/
extern void zvSpriteGAP(zVirtualScreen *VScreen, zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite, zRasterBlock *SpriteBkgnd);


/*---------------------------------------------------------------------------
| zvSpriteGet()
|
| Used in sprite graphics or animation to retrieve the background for a 
| sprite, normally just before using zvSpritePut().  Sprite contains the 
| sprite which should have been previously defined by zvBlockGet() or similar
| function.  TransColor is the transparent color assumed in the sprite.  
| Spritebkgnd, a pointer to a zRasterBlock structure the same size as the 
| sprite, will receive the sprite's background.  The top, left corner of the 
| sprite's location is specified by P.
|
| Inputs: VScreen     - pointer to descriptor of virtual screen
|         TransColor  - color representing transparency in sprites
|         P           - location of top left corner
|         Sprite      - reference sprite
|         SpriteBkgnd - buffer to store the data below the sprite
|
| Return: none
*/
extern void zvSpriteGet(zVirtualScreen *VScreen, zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite, zRasterBlock *SpriteBkgnd);


/*---------------------------------------------------------------------------
| zvSpritePut()
|
| Used in sprite graphics or animation to display a sprite or, more commonly, 
| its background.  Sprite contains the sprite which should have been 
| previously defined by zvBlockGet(), zvSpriteGAP() or zvSpriteGet().  
| TransColor is the transparent color assumed in Sprite.  The top, left 
| corner of the sprite's location is specified by P.
|
| Inputs: VScreen    - pointer to descriptor of virtual screen
|         TransColor - color representing transparency in sprite
|         P          - location of top left corner
|         Sprite     - the sprite to draw
|
| Return: none
*/
extern void zvSpritePut(zVirtualScreen *VScreen, zbyte TransColor, z2DPoint *P, zRasterBlock *Sprite);


#ifdef __cplusplus
}
#endif


/*---------------------------------------------------------------------------
| By default Watcom does not prefix an underscore or pass arguments on the
| stack and expects functions to remove their own arguments from the stack.
| These pragmas take care of this.
*/
#ifdef __WATCOMC__
#pragma aux ZVSVGA_C "_*" parm caller [];
/* Global Functions */
#pragma aux (ZVSVGA_C) zvBlit;
#pragma aux (ZVSVGA_C) zvBlockGet;
#pragma aux (ZVSVGA_C) zvBlockPut;
#pragma aux (ZVSVGA_C) zvDrawAALine;
#pragma aux (ZVSVGA_C) zvDrawBox;
#pragma aux (ZVSVGA_C) zvDrawCirArc;
#pragma aux (ZVSVGA_C) zvDrawCircle;
#pragma aux (ZVSVGA_C) zvDrawCubicBezier;
#pragma aux (ZVSVGA_C) zvDrawEllArc;
#pragma aux (ZVSVGA_C) zvDrawEllipse;
#pragma aux (ZVSVGA_C) zvDrawFillBox;
#pragma aux (ZVSVGA_C) zvDrawFillCircle;
#pragma aux (ZVSVGA_C) zvDrawFillEllipse;
#pragma aux (ZVSVGA_C) zvDrawLine;
#pragma aux (ZVSVGA_C) zvDrawPoint;
#pragma aux (ZVSVGA_C) zvDrawString;
#pragma aux (ZVSVGA_C) zvDrawStringDn;
#pragma aux (ZVSVGA_C) zvDrawStringLt;
#pragma aux (ZVSVGA_C) zvDrawStringRt;
#pragma aux (ZVSVGA_C) zvFillArea;
#pragma aux (ZVSVGA_C) zvFillColor;
#pragma aux (ZVSVGA_C) zvFillConvexPoly;
#pragma aux (ZVSVGA_C) zvFillPoly;
#pragma aux (ZVSVGA_C) zvFillScreen;
#pragma aux (ZVSVGA_C) zvFillView;
#pragma aux (ZVSVGA_C) zvGetPoint;
#pragma aux (ZVSVGA_C) zvPCXMake;
#pragma aux (ZVSVGA_C) zvPCXPut;
#pragma aux (ZVSVGA_C) zvScrollDn;
#pragma aux (ZVSVGA_C) zvScrollLt;
#pragma aux (ZVSVGA_C) zvScrollRt;
#pragma aux (ZVSVGA_C) zvScrollUp;
#pragma aux (ZVSVGA_C) zvSetView;
#pragma aux (ZVSVGA_C) zvSpriteGAP;
#pragma aux (ZVSVGA_C) zvSpriteGet;
#pragma aux (ZVSVGA_C) zvSpritePut;
#endif /* __WATCOMC__ */


/*=========================================================================*/
#endif /* ZVSVGA_H */
