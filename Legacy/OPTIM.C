#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <time.h>
#include <i86.h>
#include "sys.h"
#include "file.h"
#include "screen.h"
#include "path.h"
#include "problem.h"
#include "optim.h"

#define DEBUG 0

int Same(int A,int B,int C, int D)
{
    return ( ((A==D) && (B==C)) || ((A==C) && (B==D)) );

}

int NextOneOpt(PROBLEM *prob,int *next,int *oA,int *oB,int *nA,int *nB)
// * returns 1 if successfully finds one, 0 otherwise * //
// * sets next, A & B being place to insert next      * //
{

    int *path=PathToArray(prob->path); /* list of all to check */
    int max=prob->path->size;
    int insert,A,B,C,D,I;
    double removal,insertion,thisimp,improvement=0.0;

    for (insert=0;insert<max;insert++) // * run thru all cities * //
    {
            I=path[insert];
            TmpPathDel(prob->path,I,&A,&B);
            if (DEBUG) printf("Checking %d from %d->%d\n",I,A,B);


            removal=(prob->dist->array[A][B])
                   -(prob->dist->array[I][A])
                   -(prob->dist->array[I][B]);
            if (DEBUG) printf("Removal is %.4f\n",removal);
            
            BestInsert(prob,I,&C,&D);
            insertion=InsertCost(prob,I,C,D);
            if (DEBUG) printf("Insertion between %d,%d is %.4f\n",C,D,insertion);
            thisimp=insertion+removal;
            if (DEBUG) printf("This %.4f Best %.4f\n",thisimp,improvement);
            if ((thisimp<improvement) && (!(Same(A,B,C,D))) )
            {
                improvement=thisimp;
                *next=I;
                *oA=A;
                *oB=B;
                *nA=C;
                *nB=D;
                
            }
            TmpPathIns(prob->path);
    }
    if (improvement>=0.0) return 0;
    return 1;
}

void ControlOneOpt(PROBLEM *prob)
{
    int next,oldA,oldB,newA,newB,times=0;
    char buffer[50];
    double oldpl=PathLength(prob->path,prob->dist);
    
    while (NextOneOpt(prob,&next,&oldA,&oldB,&newA,&newB))
    {

        sprintf(buffer,"Run %d\n",times);
        ShowText(buffer,1,1,BLUE);


        HiliteCityGroup(prob,next,YELLOW,oldA,oldB,BLUE);
        delay(100);
        HiliteCityGroup(prob,next,YELLOW,newA,newB,BLUE);
        delay(100);

        sprintf(buffer,"%2d to %2d,%2d",next,newA,newB);
        ShowText(buffer,1,2,GREEN);

        DeletePath(prob->path,next);
        InsertPath(prob->path,next,newA,newB);
        DrawNorm(prob,1);
        delay(100); 
            
         ++times;
    }
    sprintf(buffer,"Lengths\n"
                   "Old = %.6f\n"
                   "New = %.6f\n",oldpl,PathLength(prob->path,prob->dist));
    ShowText(buffer,1,6,YELLOW);

}

void NextTwoOpt(PROBLEM *prob)
{
    

}
