#include <stdlib.h>
#include <stdio.h>
#include <graph.h>
#include <time.h>
#include <conio.h>
#include <i86.h>
#include <dos.h>
#include "sys.h"
#include "screen.h"
#include "file.h"
#include "path.h"
#include "problem.h"
#include "anneal.h"

void main(int argc,char *argv[])
{
    char *filename=argv[1];
    ANNPROB *annp=NewAnnProb(NewMap(NewLocates(filename)),opt1);
    int x,max=annp->prob->dist->size;

    max*=(max-1);
    annp->prob->path=LocatesAsPath(annp->prob->dist->l);
    
    InitScreen(16);
    AnnWinManager(text_top);
    AnnShowText("1opt vs 3opt Annealing Run...",1,2,GREEN);
    
    AnnWinManager(text_one);
    AnnShowText("Left",1,1,RED);
    AnnWinManager(text_two);
    AnnShowText("Right",1,1,RED);
    AnnWinManager(win_one);
    DrawNorm(annp->prob,1);
    BorderWin();
    AnnWinManager(win_two);
    DrawNorm(annp->prob,1);
    BorderWin();
    SwapPage();
    getch();
    ShutDown();

    AnnealOneOpt(annp);
    ApplyTemperature(annp,1000.0);
    for (x=0;x<max;x++)
    {
        printf("I=%d A=%d B=%d C=%d D=%d DeltaE=%8.5f P=%8.5f SumP=%8.5f\n",
        annp->ann[x].array[0],
        annp->ann[x].array[1],
        annp->ann[x].array[2],
        annp->ann[x].array[3],
        annp->ann[x].array[4],
        annp->ann[x].deltaE,
        annp->ann[x].P,
        annp->ann[x].SumOfPs);
    }
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));
    printf("%d\n",ChooseChange(annp));

}
 
