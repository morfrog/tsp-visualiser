#include <graph.h>
#include <stdio.h>
#include <conio.h> /* getch */
#include <malloc.h> /* malloc,free */
#include <i86.h> /* sound */
#include "sys.h"
#include "screen.h" 

struct videoconfig  VC;
int TextColour;
int TextColour2;
int BorderColour;
int TitleColour;

/*
void main()
{
    char str[40];
    double tmp;
    SWIN *main=NewWin(1,1,480,480);
    SWIN *text=NewWin(480,10,640,170);
    
    printf("Mem Free = %d size_t\n",(int)_memavl());
    getch();
    
    InitScreen(16);

    _setcolor(line_col);

    SetWin(main);
    BorderWin();

    _setcolor(line_col);
    DrawLine(0.5,0.2,0.8,0.2);
    DrawLine(0.8,0.2,0.5,0.7);
    _setcolor(city_col);
    DrawCity("A",0.5,0.2,citysize);
    DrawCity("B",0.8,0.2,citysize);
    DrawCity("C",0.5,0.7,citysize);
    DrawCity("D",0.2,0.2,citysize);

    SetTextWin(text,1);

    sprintf(str,"Inserting %s Between %s and %s","C","A","B");
    DrawText(str,1,5,BLUE);
    sprintf(str,"Inserting %s At %s End","D","B");
    DrawText(str,-1,6,RED);

    getch();
    ShutDown();
    FreeWin(main);
    FreeWin(text);
    printf("Mem Free = %d Bytes\n",(int)_memavl());
    getch();

}
*/

int _FAR InitScreen(int videomode)
/*===========================

    This routine selects the best video mode for a given adapter.
    It also initialises screen related items.
    */

{
    int                 mode;

#if defined( __PC98__ )
    mode = _MAXCOLORMODE;
#else
    if (videomode>=0) mode = videomode;
    else
    {
    _getvideoconfig( &VC );
    switch( VC.adapter ) {
    case _VGA :
    case _SVGA :
        mode = _VRES16COLOR;
        break;
    case _MCGA :
        mode = _MRES256COLOR;
        break;
    case _EGA :
        if( VC.monitor == _MONO ) {
            mode = _ERESNOCOLOR;
        } else {
            mode = _ERESCOLOR;
        }
        break;
    case _CGA :
        mode = _MRES4COLOR;
        break;
    case _HERCULES :
        mode = _HERCMONO;
        break;
    default :
        return( 0 );          /* report insufficient hardware */
    }
#endif
    }
    
    if( _setvideomode( mode ) == 0 ) {
        return( 0 );
    }
    _getvideoconfig( &VC );
    if( VC.numcolors < 4 ) {
        TextColour = 1;
        TextColour2 = 1;
        BorderColour = 1;
    } else {
        TextColour = 1;
        TextColour2 = 3;
        BorderColour = 2;
    }
#if defined( __PC98__ )
    /* set up new colours */
    _remappalette( TextColour, _98BLUE );       /* light blue */
    _remappalette( TextColour2, _98BLUE );      /* light blue */
    _remappalette( BorderColour, _98BLACK );    /* black      */
#else
    if( VC.adapter >= _MCGA ) {
        /* set up new colours */
        _remappalette( TextColour, 0x3f0000 );  /* light blue */
        _remappalette( TextColour2, 0x3f0000 ); /* light blue */
        _remappalette( BorderColour, _BLACK );  /* black      */
    }
#endif
    _setwindow(0,0.0,0.0,1.0,1.0);
    _setcharsize_w(citysize*4/3,citysize*4/3);
    _registerfonts( "*.fon" );
    return( 1 );
}

int _FAR ShutDown( void )
{
    _unregisterfonts();
    _setvideomode( _DEFAULTMODE );      /* reset the screen */

    return 1;
}

SWIN _FAR *NewWin(int l,int t,int r,int b)
{
    SWIN *w=NULL;
    w=malloc(sizeof(SWIN));
    w->t=t;
    w->l=l;
    w->b=b;
    w->r=r;
    return w;
}

void _FAR SetWin(SWIN *w)
{
    _setviewport(w->t,w->l,w->b,w->r);
    _setcharsize_w(citysize*4/3,citysize*4/3);
    _setcharspacing_w(-0.02);
}

void _FAR SetTextWin(SWIN *w,int border)
{
    int px=VC.numxpixels/VC.numtextcols; /* pixels / col */
    int py=VC.numypixels/VC.numtextrows; /* pixels / col */
    _settextwindow((w->t)/py,(w->l)/px+2,(w->b)/py,(w->r)/px+2);
    /* +2 is roundup + gap */
    if (border) _rectangle(_GBORDER,w->l,w->t,w->r,w->b);
}

void _FAR ResetWin(SWIN *w,int t,int l,int b,int r)
/* resets the window to the cooords specified, -1 indicates no change */
{
    if (t>=0) w->t=t;
    if (t>=0) w->l=l;
    if (t>=0) w->b=b;
    if (t>=0) w->r=r;
}

void _FAR BorderWin()
{
    _setcolor(border_col);
    _rectangle_w(_GBORDER,0.0,0.0,1.0,1.0);
}

void _FAR FreeWin(SWIN *w)
{
    if (!(sfree(w))) printf("Failed To Free Window.\n");
}

double _FAR Scale(int thing,int max)
{
    return (double)thing/(double)max;
}

void _FAR DrawLine(double x1,double y1,double x2,double y2)
/* Draw A line in the current window 
 *  pre  : 0.0 < x1,x2 < 1.0
 *         0.0 < y1,y2 < 1.0 
 *  post : scales points and draws line in <out> box
 */
{
        _setcolor(line_col);
        _moveto_w(x1,y1);
        _lineto_w(x2,y2);
}

void _FAR DrawCity(char name[],double x,double y,double size)
/* x,y,size are in unit square 0.0,0.0 - 1.0,1.0 */
{
    double t=y-size,l=x-size,b=y+size,r=x+size;
    _setcolor(city_col);
    _ellipse_w(_GFILLINTERIOR,l,t,r,b);
    _setcolor(text_col);
    _settextalign(_CENTER,_HALF);
    _grtext_w(x, y, name);
}

void _FAR TextWindow(int rows,int cols)
{
    _settextwindow(12,12,rows,cols);
}

void _FAR DrawText(char string[],int start,int line,int color)
/* Draws text in the current text box, -ve start = continue last pos
 * pre  : line -> [1..6], could use <1 to have higher on screen
 * post : string is displayed at bottom of screen
 */
{
    _settextcolor(color);
    if (start>=0) _settextposition(line,start);
    _outtext(string);
}

void _FAR SwapPage()
{
    static int viewpage=0;
    _setactivepage(viewpage);
    if (viewpage) viewpage=0;
    else          viewpage=1;
    _setvisualpage(viewpage);
}
