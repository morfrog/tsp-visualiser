#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
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

int ProcessCmdLine(int argc,char *argv[],char **filein,char **fileout,int *method)
{
    if (argc<4) {
        printf("Visualisation Tool For The Travelling Salesman Problem\n"
               "Optimization Methods. V%3.2f\n",VERSION);
        printf("Usage: %s input output method \n%s",
                argv[0],
                "Where  input   = input filename\n"
                "       output  = output filename\n"
                "       method in {1opt,2opt,3opt}\n"
                "       1opt being Single City Move\n"
                "       2opt being Segment Reversal\n"
                "       3opt being ???\n");

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
        return 1;
    }
}

void main(int argc,char *argv[])
{
    int x;
    PROBLEM *prob;
    char *filein,*fileout;
    int method;

    if (ProcessCmdLine(argc,argv,&filein,&fileout,&method)) {
        prob=NewProblem(NewMap(NewLocates(filein)));
        prob->path=LocatesAsPath(prob->dist->l);

        InitScreen(16);

        
        SwapPage();
        WinManager(Main);
        WinManager(Text);

        TotalDelay(400);

        DrawNorm(prob,1);
        delay(1000);
        ControlOneOpt(prob);
        delay(1000);

        if (DEBUG) ShowPath(prob->path);

        ShutDown();
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
