#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include <conio.h>
#include "sys.h"
#include "path.h"
#include "file.h"
#include "problem.h"
#include "screen.h"
#include "optim.h"

#define VERSION 1.01

// #define DEBUG 0

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
               "       time    =-3 optimal is Benchmark for all methods,\n"
               "                              with optimal calculation\n"
               "       No time = wait for keypress between each optimize step\n");
        printf("E.G. %s myfile.in myfile.out 2opt",argv[0]);

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
    PROBLEM *prob1,*prob2,*prob3;
    LOCATES *out=NULL;
    DISTANCES *dtemp;
    char *filein,*fileout;
    int method,tdelay;
    double optimal,length,oldpl;
    char buffer[50];

    if (ProcessCmdLine(argc,argv,&filein,&fileout,&method,&tdelay,&optimal)) {
        TotalDelay(tdelay);
        dtemp=NewMap(NewLocates(filein));
        prob1=NewProblem(dtemp);
        prob1->path=LocatesAsPath(prob1->dist->l);

        if (tdelay==-3)
        {
            prob2=NewProblem(dtemp);
            prob2->path=LocatesAsPath(prob2->dist->l);
            prob3=NewProblem(dtemp);
            prob3->path=LocatesAsPath(prob3->dist->l);

            printf("             1opt     2opt     3opt\n"
                   "%12s ",filein);
            printf("%8.5f  %8.5f  %8.5f\n",ControlOneOpt(prob1,0)/optimal,
            ControlTwoOpt(prob2,0)/optimal,Control3Opt(prob3,0)/optimal);
        }
        else if (tdelay==-2) 
        {
            length=ControlAllOpt(prob1,method,((tdelay==-1) || (tdelay>0)),tdelay);
            printf("Length was %8.6f\n",length);
        }
        else
        {
        
            InitScreen(16);
    
            SwapPage();
            WinManager(Main);
            WinManager(Text);

            if (tdelay==0)
                ShowText("Working...\n\n"
                         "Working...\n",1,20,BLUE);
            else
                ShowText("Hit Esc to Quit,\n\n"
                 "Any other key \n"
                 "To continue..",1,20,BLUE);
                
            DrawNorm(prob1,1);
            SwapPage();
            oldpl=PathLength(prob1->path,prob1->dist);
            
            length=ControlAllOpt(prob1,method,((tdelay==-1) || (tdelay>0)),tdelay);

            DrawNorm(prob1,1);
            SwapPage();

            ShowText("F I N I S H E D",1,13,RED);
            sprintf(buffer,"Lengths\n"
                   "Old = %.6f\n"
                   "New = %.6f\n",oldpl,length);
            ShowText(buffer,1,14,YELLOW);


            
#ifdef DEBUG
    ShowPath(prob1->path);
#endif
    
            ShowText("Hit Esc         \n"
                     "to Quit...      \n",1,19,LTRED);
            ShowText("                \n"
                     "                ",1,21,BLACK);
            
            while (getch()!=27) {}
            
            ShutDown();
            
        }    
    
        
        out=PathAsLocates(prob1->path,prob1->dist->l);
        WriteLocates(out,fileout);
        FreeLocates(out);

        
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
