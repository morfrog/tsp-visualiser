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
        ControlAnneal(NewProblem(NewMap(NewLocates(filename))),temperature,cooling,changes,tdelay);
        ShutDown();
    }
}
