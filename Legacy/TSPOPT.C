#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include "sys.h"
#include "path.h"
#include "file.h"
#include "problem.h"
#include "screen.h"
#include "optim.h"

#define VERSION 1.01

#define DEBUG 0

#define opt1 1
#define opt2 2
#define opt3 3

int ProcessCmdLine(int argc,char *argv[],char **filein,char **fileout,int *method,int *tdelay,double *optimal)
{
    int wait_time;
    if (argc<4) {
        printf("Visualisation Tool For The Travelling Salesman Problem\n"
               "Optimization Methods. V%3.2f\n",VERSION);
        printf("Usage: %s input output method [time]\n%s",
                argv[0],
                "Where  input   = input filename\n"
                "       output  = output filename\n"
                "       method in {1opt,2opt,3opt}\n"
                "       time    = 0 is calculate to end\n"
                "       time    = n is take n 10ths of seconds for each insert\n"
                "       time    =-2 is Benchmark Mode - Simply returns length\n"
                "       time    =-3 double is Benchmark for all methods, double=optimal\n"
                "       No time = wait for keypress between each insert\n");


        return 0;
    }
    else
    {
        *filein=argv[1];
        *fileout=argv[2];
        switch (argv[3][0]-48) {
            case opt1 : *method=opt1; break;
            case opt2 : *method=opt2; break;
            case opt3 : *method=opt3; break;
            default : printf("Invalid Method. Method must be in {1opt,2opt,3opt}");
                      return 0;break;
        }
        if (argc>=5) // * optional time taken * //
        {
            wait_time=atoi(argv[4]);
            switch (wait_time) {
                case 0 : *tdelay=0;  break;
                case -2: *tdelay=-2; break; // * benchmark only - no graphics
                case -3: *tdelay=-3; *optimal=(double) atof(argv[5]); break;
                default: *tdelay=wait_time*(CLOCKS_PER_SEC/10); break;
            }
        }
        else *tdelay=-1; // * optional time not specified * //

        return 1;

    }
}

void main(int argc,char *argv[])
{
    int x;
    PROBLEM *prob;
    LOCATES *out=NULL;
    char *filein,*fileout;
    int method,y,max=6,*i,tdelay;
    double optimal,length;

    if (ProcessCmdLine(argc,argv,&filein,&fileout,&method,&tdelay,&optimal)) {
        TotalDelay(tdelay);
        prob=NewProblem(NewMap(NewLocates(filein)));
        prob->path=LocatesAsPath(prob->dist->l);

        if (tdelay==-3)
        {
            printf("             1opt     2opt     3opt\n"
                   "%12s ",filein);
            printf("%8.6f %8.6f\n",
                    ControlOneOpt(prob,0)/optimal,
                    ControlTwoOpt(prob,0)/optimal);

            
        }
        else
        {
            if (tdelay>=-1) {
                InitScreen(16);
    
                SwapPage();
                WinManager(Main);
                WinManager(Text);
    
                DrawNorm(prob,1);
                delay(1000);
            }
            switch (method) {
                case opt1 : length=ControlOneOpt(prob,((tdelay==-1) || (tdelay>0)));break;
                case opt2 : length=ControlTwoOpt(prob,((tdelay==-1) || (tdelay>0)));break;
                case opt3 : break;
                default : break;
            }
            if (tdelay>=-1) {
                DrawNorm(prob,1);
                delay(1000);
                if (DEBUG) ShowPath(prob->path);
    
                mygetch();
                ShutDown();
            }
            if (tdelay==-2) printf("Length was %8.6f\n",length);
    
        
            out=PathAsLocates(prob->path,prob->dist->l);
            WriteLocates(out,fileout);
            FreeLocates(out);

        }
    }
}


/*
void main()
{
    PATH *test;
    int x;

    test=NewPath(4,0,2,1,3);
    printf("Start path is ");ShowPath(test);
    for (x=0;x<=3;x++) {
        printf("Tmp Del is %d  |  ",x);
        TmpPathDel(test,x);
        ShowPath(test);
        TmpPathIns(test);
        ShowPath(test);
    }
}

*/
