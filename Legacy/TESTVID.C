#include <stdio.h>
#include <graph.h>
#include "conio.h"

_remappalette

void main()
{
    struct videoconfig  VC;
    int x;
    
    for (x=-3;x<20;x++)
    {
        _setvideomode(x);
        _getvideoconfig(&VC);
        if (VC.numxpixels!=0)
        {
            printf("Mode  X   Y  Col Page Mem\n");
            printf("%3d  %3d %3d %3d %3d  %3d\n",
            x,VC.numxpixels,VC.numypixels,VC.numcolors,VC.numvideopages,VC.memory);
            getch();
        }

    }
    for (x=0x100;x<=0x105;x++)
    {
        _setvideomode(_VRES256COLOR);
        _getvideoconfig(&VC);
        printf("Mode  X   Y  Col Page Mem\n");
        printf("%3d  %3d %3d %3d %3d  %3d\n",
        x,VC.numxpixels,VC.numypixels,VC.numcolors,VC.numvideopages,VC.memory);
        getch();
    }   
    _setvideomode(-1);    
    printf("Clean Exit\n");
    getch();
}

