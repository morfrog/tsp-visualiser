#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <conio.h>
#include "sys.h"
#include "screen.h"


void main()
{
    SWIN *win=NewWin(1,1,200,200);
    char buffer[40];
    InitScreen(16);
    _setvisualpage(0);
    _setactivepage(0);
    BorderWin(win,YELLOW);
    
    SetTextWin(win);
    DrawText("God of all that is good and right",1,1,GREEN);
    DrawText("Hello God of all that is seen and heard",1,3,RED);
    getch();
}
