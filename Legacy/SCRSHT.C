#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include "sys.h"

void main(int argc,char *argv[])
{
    char *filename=argv[1];
    FILE *input;
    int x,y,color;
    
    printf("Converting '%s'",filename);
    input=fopen(filename,"rb");

    _setvideomode(16);
    for (x=0;x<640;x++)
        for (y=0;y<350;y++)
        {
            color=fgetc(input);
            if (color==0) color=15;
            else if (color==15) color=0;
            _setcolor(color);
            _setpixel(x,y);
        }
    getch();
    _setvideomode(_DEFAULTMODE);

    printf("Done...");
}
