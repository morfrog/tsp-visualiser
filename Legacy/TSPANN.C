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

#define VERSION 1.01

int ProcessCmdLine(int argc,char *argv[],char **filename,double *temp,double *cool,int *runsattemp,int *tdelay,int *optimal)
{
    int wait_time,x,y,z,c;
    if (argc<4) {
        printf("Visualisation Tool For The Travelling Salesman Problem - Annealing Methods\n"
               "By Drew   9/97           V%3.2f\n\n",VERSION);
        
        printf("Usage: %s viewfile temp cool changes [time]\n%s",
                argv[0],
               "Where  viewfile= the file to run annealing on\n"
               "       temp    = the starting temperature\n"
               "       cool    = the cooling rate (0.0 < rate < 1.0)\n"
               "       changes = the number of changes to make at each temperature\n" 
               "       time    = 0 is calculate to end\n"
               "       time    = n is take n 10ths of seconds for each insert\n"
               "       time    =-2 is Benchmark Mode - Simply returns length\n"
               "       time    =-3 optimal is Benchmark for all methods,\n"
               "                              with optimal calculation\n"
               "       No time = wait for keypress between each optimize step\n");
        printf("E.G. %s myfile.in 1000.0 0.8 10 1\n",argv[0]);
/*
    for (z=3;z<12;z++)
    {
        c=0;
        for (x=0;x<z-2;x++)
            for (y=x+2;y<z;y++)
                if (!((x==0) && (y==z-1)))
                {
                    printf("%d %d %d %d\n",x,x+1,y,y==z-1?0:y+1);
                    ++c;
                }
        printf("%d maps to %d is %d\n___________\n",z,c,PlusFactorial(z-2)-1);
    }
*/
        return 0;
    }
    else
    {
        *filename=argv[1];
        *temp=atof(argv[2]);
        *cool=atof(argv[3]);
        if ((*cool<=0.0) || (*cool>=1.0)) {
            printf("%3f is an invalid cooling rate. (0.0 < cooling rate < 1.0)\n",*cool);
            exit(0);
        }
        *runsattemp=atoi(argv[4]);
        if (*runsattemp<=0) {
            printf("Must have more than %d runs at temp.\n",*runsattemp);
            exit(0);
        }


        if (argc>=6) // * optional time taken * //
        {
            wait_time=atoi(argv[5]);
            switch (wait_time) {
                case 0 : *tdelay=0;  break;
                case -2: *tdelay=-2; break; // * benchmark only - no graphics
                case -3: *tdelay=-3; *optimal=(double) atof(argv[6]); break;
                default: *tdelay=wait_time*(CLOCKS_PER_SEC/10); break;
            }
        }
        else *tdelay=-1; // * optional time not specified * //
        return 1;
    }
}

void main(int argc,char *argv[])
{
    char *filename;
    double temperature,cooling,optimal;
    int changes,tdelay;
    ANNPROB *leftp;
    ANNPROB *rightp;
    char buffer[80];
    char ch;
          
    if (ProcessCmdLine(argc,argv,&filename,&temperature,&cooling,&changes,&tdelay,&optimal))
    {
        leftp=NewAnnProb(NewMap(NewLocates(filename)),opt1);
        rightp=NewAnnProb(NewMap(NewLocates(filename)),opt2);
        leftp->prob->path=LocatesAsPath(leftp->prob->dist->l);
        rightp->prob->path=LocatesAsPath(rightp->prob->dist->l);

        TotalDelay(tdelay);
        InitScreen(16); // * 2 pages, 640x350, 16 color * //
        AnnWinManager(text_top);

        sprintf(buffer,"Temperature=%11.6f  Cooling_Rate=%8.6f  "
                "#Changes=%3d OF %3d.",temperature,cooling,1,changes);
        ShowText("1opt vs 2opt Annealing Run...",1,1,GREEN);
        ShowText(buffer,1,3,RED);

        AnnWinManager(text_one);
        ShowText("Length=            Accepted=       %",1,1,RED);
        AnnWinManager(text_two);
        ShowText("Length=            Accepted=       %",1,1,RED);

        AnnWinManager(win_one);
        DrawNorm(leftp->prob,1);
        AnnWinManager(win_two);
        DrawNorm(leftp->prob,1);
        SwapPage();

        ControlAnneal(leftp,rightp,temperature,cooling,changes);

        AnnWinManager(win_one);
        DrawNorm(leftp->prob,1);
        AnnWinManager(win_two);
        DrawNorm(rightp->prob,1);
        SwapPage();
        AnnWinManager(text_top);
        ShowText("Hit Esc To Quit, Any Key For Overlay.",35,1,YELLOW);
        ch=getch();
        if (ch!=27)
        {
            AnnWinManager(text_one);
            sprintf(buffer,"Length=%f  ",
                PathLength(leftp->prob->path,leftp->prob->dist));
            ShowText(buffer,1,1,YELLOW);

            AnnWinManager(text_two);
            sprintf(buffer,"Length=%f  ",
                PathLength(rightp->prob->path,rightp->prob->dist));
            ShowText(buffer,1,1,LTRED);

            ControlLCLOverlay(leftp->prob,rightp->prob,639,240,59,YELLOW,LTRED);
/*
            ControlVarOverlay(leftp->prob,1,59,639/2,298,LTRED,0,3,-4,-3,IntLesser,40,40,-1);
            ControlVarOverlay(rightp->prob,321,59,639,298,ORANGE,4,3,0,-3,IntLesser,40,40,-1);
            ControlVarOverlay(rightp->prob,310,170,329,190,YELLOW,-4,-3,4,3,IntGreater,640,349,-1);
            ControlVarOverlay(rightp->prob,2,1,20,2,GREEN,0,0,4,4,IntGreater,640,349,-1);
            ControlVarOverlay(rightp->prob,560,2,639,81,BLUE,-4,1,-4,2,IntLesser,78,78,130);
            ControlVarOverlay(rightp->prob,0,0,640,350,LTRED,4,3,-4,-3,IntLesser,40,40,-1);

            while (1) {
                ControlVarOverlay(rightp->prob,0,0,640,350,GREEN,4,0,-4,0,IntLesser,-1,-1,160);
                ControlVarOverlay(rightp->prob,0,0,640,350,GREEN,0,4,0,-4,IntLesser,-1,-1,87);
            }
            getch();
*/
            if (kbhit()) getch();

        }

        ShutDown();
    }
}
 
