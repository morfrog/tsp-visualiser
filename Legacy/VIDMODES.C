
#include <stdio.h>
#include <graph.h>
#include <conio.h>

#define size 150

void main()
{
    struct videoconfig  VC[size];
    int mode,current=0;
    for (mode=-10;mode<30;mode++) {
        _setvideomode( mode );
        _getvideoconfig( &VC[current] );
        ++current;
    }
    _setvideomode( 0x100);
    _getvideoconfig(&VC[current]);
    ++current;
    _setvideomode( 0x101);
    _getvideoconfig(&VC[current]);
    ++current;
    _setvideomode( 0x102);
    _getvideoconfig(&VC[current]);
    ++current;
    _setvideomode( 0x103);
    _getvideoconfig(&VC[current]);
    ++current;
    _setvideomode( 0x104);
    _getvideoconfig(&VC[current]);
    ++current;
    _setvideomode( 0x105);
    _getvideoconfig(&VC[current]);
    ++current;


    _setvideomode(_DEFAULTMODE);
    current=0;
    for (mode=-10;mode<30;mode++) {
        if (VC[current].numvideopages>1)
        {
            printf("mode %d :- pixels=%dx%d text=%dx%d cols=%d pages=%d\n",
            mode,
            VC[current].numxpixels,
            VC[current].numypixels,
            VC[current].numtextcols,
            VC[current].numtextrows,
            VC[current].numcolors,
            VC[current].numvideopages);
        }
        ++current;
    }
    for (mode=100;mode<=105;mode++)
    {
            printf("mode 0x%d :- pixels=%dx%d text=%dx%d cols=%d pages=%d\n",
            mode,
            VC[current].numxpixels,
            VC[current].numypixels,
            VC[current].numtextcols,
            VC[current].numtextrows,
            VC[current].numcolors,
            VC[current].numvideopages);
            ++current;        
    }
    getch();
    _setvideomode(16);
    _setactivepage(0);
    _moveto(1,1);
    _lineto(100,100);
    _setactivepage(1);
    _moveto(100,100);
    _lineto(200,200);
    _setvideomode(13);
    _moveto(1,1);
    _lineto(100,1);
    getch();
    _setvideomode(16);
    _setvisualpage(0);
    getch();
    _setvisualpage(1);
    getch();

}
