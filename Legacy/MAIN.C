#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <graph.h>
#include <i86.h>
#include "sys.h"
#include "screen.h"
#include "readmap.h"

void DrawCities(LOCATES *loc)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<=(loc->num)-1;count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
                DrawCity(num,loc->at[count].x,loc->at[count].y,0.03);
    }
}

void main()
{
    int *t;
    int **u;
    int num=0,count=0;
    LOCATES *l=GetLocates();
    SWIN *main;
    int max=l->num,x,y;
    struct videoconfig VC;
    
    ShowLocates(l);

    InitScreen(16);
    main=NewWin(1,1,480,480);

/*
    for (x=0;x<21;x++)
    {
        _setvideomode(x);
        _getvideoconfig(&VC);
        printf("%d has %d pages (%d,%d).\n",x,VC.numvideopages,VC.numxpixels,
        VC.numypixels);
        delay(2000);
    }
*/
    
    SetWin(main);
    BorderWin();
    DrawCities(l);
    _setvisualpage(0);
    _setactivepage(1);
    BorderWin();
    for (x=0;x<max;x++)
        for (y=0;y<max;y++)
            if (x!=y)
                DrawLine(l->at[x].x,l->at[x].y,l->at[y].x,l->at[y].y);

    DrawCities(l);
    delay(2000);
    _setvisualpage(1);
    delay(4000);
    ShutDown();

    u=allocate_N_by_N_array(20,20,sizeof(int),"u");
    for (x=0;x<20;x++)
        for (y=0;y<20;y++)
        {
            u[x][y]=num;
            ++num;
        }
    delay(1000);
    for (x=0;x<20;x++)
        for (y=0;y<20;y++)
        {
            if (count!=u[x][y]) printf("%d!=%d\n",count,u[x][y]);
            else printf("%d ",u[x][y]);
            ++count;
            
        }
   
    
}
