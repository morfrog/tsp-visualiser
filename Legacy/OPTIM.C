#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <time.h>
#include <i86.h>
#include <graph.h>
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

    int *path=PathToArray(prob->path,0); /* list of all to check */
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

double ControlOneOpt(PROBLEM *prob,int graphics)
{
    int next,oldA,oldB,newA,newB,times=0;
    char buffer[50];
    double oldpl=PathLength(prob->path,prob->dist),newpl;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    
    if (graphics) {
        sprintf(buffer,"One Opt Method\n");
        ShowText(buffer,1,1,GREEN);
    }
    while (NextOneOpt(prob,&next,&oldA,&oldB,&newA,&newB))
    {

        now=clock();

        if (graphics) {
            sprintf(buffer,"Run %d\n",times);
            ShowText(buffer,1,3,BLUE);
            ++times;
            HiliteCityGroup(prob,next,YELLOW,oldA,oldB,BLUE);
            WaitTill(now+(time_seg));
    
            HiliteCityGroup(prob,next,YELLOW,newA,newB,BLUE);
            WaitTill(now+(time_seg*2));
            sprintf(buffer,"%2d to %2d,%2d",next,newA,newB);
            ShowText(buffer,1,4,GREEN);
        }

        DeletePath(prob->path,next);
        InsertPath(prob->path,next,newA,newB);

        if (graphics) {
            DrawNorm(prob,1);
            WaitTill(now+(time_seg*3));
        }
            
        
    }
    newpl=PathLength(prob->path,prob->dist);
    if (graphics) {
        sprintf(buffer,"Lengths\n"
                   "Old = %.6f\n"
                   "New = %.6f\n",oldpl,newpl);
        ShowText(buffer,1,6,YELLOW);
    }
    return newpl;
}

int NextTwoOpt(PROBLEM *prob,int *A,int *B,int *C,int *D)
{
    int *array=PathToArray(prob->path,1);
    int max=prob->path->size; // * 1 = extension * //
    int mA,mB,mC,mD,x,y;
    double change,best=0.0;

    for (x=0;x<max-2;x++)
        for (y=x+2;y<max;y++)
        {
            mA=array[x];mB=array[x+1];
            mC=array[y];mD=array[y+1];

            if (mA!=mD) {
                if (DEBUG) printf("%d %d | %d %d\n",mA,mB,mC,mD);

                change=prob->dist->array[mA][mC]+prob->dist->array[mB][mD]
                      -prob->dist->array[mA][mB]-prob->dist->array[mC][mD];
                if (change<best)
                {
                    *A=mA;
                    *B=mB;
                    *C=mC;
                    *D=mD;
                    best=change;
                }
            }
        }
    /*s*/free(array);
    return (best!=0.0);
}

void Swap(int *A,int *B)
{
    int tmp=*A;
    *A=*B;
    *B=tmp;
}

void Reverse(int *array,int size,int from,int to)
{
    int start=PosOf(from,array,size);
    int end=PosOf(to,array,size);
    int tmp,elems,swaps,x;

    if (end<start) Swap(&end,&start);

    elems=(end-start+1);
    swaps=((elems / 2) + (elems % 2));
    for (x=0;x<swaps;x++) Swap(&array[start+x],&array[end-x]);
}

void Test()
{
    int list[9]={1,2,3,4,5,6,7,8,9},x;

    printf("\n1 2 7 6 5 4 3 8 9\n");
    Reverse(list,9,3,7);
    for (x=0;x<9;x++)
        printf("%d ",list[x]);

    printf("\n1 2 3 4 5 6 7 8 9\n");
    Reverse(list,9,1,7);
    for (x=0;x<9;x++)
        printf("%d ",list[x]);

}

void MakeTwoOptTransition(PROBLEM *p,int A,int B,int C,int D)
{
    int *array;
    int type=0,x,size=p->path->size;
    int start=-1,end=-1;

/*    
    if (DEBUG) printf("---------MakeTwoOptTransition---------\n");
    array=PathToArray(p->path,1);
    start=B;end=C;
    if (DEBUG) printf("%d %d Reversing...\n",start,end);
    Reverse(array,size,start,end);
    FreePath(p->path);
    p->path=ArrayToPath(array,size,0);
*/   
    if (DEBUG) {printf("%d,%d,%d,%d ",A,B,C,D);ShowPath(p->path);}
    ReverseSubPath(p->path,A,B,C,D);
   
}

void HiliteTwoOpt(PROBLEM *prob,int A,int B,int C,int D,int lines)
{
    LOCATES *l=prob->dist->l;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);
    if (lines)
    {
        DrawCitiesLine(l,A,B,BLACK);
        DrawCitiesLine(l,C,D,BLACK);
        DrawCitiesLine(l,A,C,city_hi);
        DrawCitiesLine(l,B,D,city_hi);
    }

    SetMasks(prob->vmask,4,A,city_hi,citysize*2,
                           B,city_hi,citysize*2,
                           C,city_hi,citysize*2,
                           D,city_hi,citysize*2);

    DrawCities(l,prob->vmask);
    
    SetMasks(prob->vmask,4,A,city_in,citysize,
                           B,city_in,citysize,
                           C,city_in,citysize,
                           D,city_in,citysize);
    BorderWin();
    SwapPage();
}


double ControlTwoOpt(PROBLEM *prob,int graphics)
{
    int A,B,C,D;
    char buffer[80];
    double oldpl=PathLength(prob->path,prob->dist),newpl;
    double dab,dcd,dac,dbd;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;

    if (DEBUG) ShowPath(prob->path);
    if (graphics) {
        sprintf(buffer,"Two Opt Method\n");
        ShowText(buffer,1,1,GREEN);
    }
    while (NextTwoOpt(prob,&A,&B,&C,&D))
    {
        now=clock();
        if (DEBUG) printf("Next is %d %d to %d %d\n",A,B,C,D);
        if (graphics) {
            HiliteTwoOpt(prob,A,B,C,D,0);
            sprintf(buffer,"From %2d,%2d\n"
                           "To   %2d,%2d\n",A,B,C,D);
                           
            ShowText(buffer,1,3,GREEN);
            dab=prob->dist->array[A][B];
            dcd=prob->dist->array[C][D];
            dac=prob->dist->array[A][C];
            dbd=prob->dist->array[B][D];
            sprintf(buffer,"%2d->%2d = %8.6f\n"
                           "%2d->%2d = %8.6f\n"
                           "%2d->%2d = %8.6f\n"
                           "%2d->%2d = %8.6f\n",
                           A,B,dab,
                           C,D,dcd,
                           A,C,dac,
                           B,D,dbd);
            ShowText(buffer,1,5,BLUE);
            sprintf(buffer,"Saving is %8.6f\n",dac+dbd-dab-dcd);
            ShowText(buffer,1,9,BLUE);
            WaitTill(now+(time_seg));
            HiliteTwoOpt(prob,A,B,C,D,1);
            WaitTill(now+(2*time_seg));
        }
        MakeTwoOptTransition(prob,A,B,C,D);
        if (graphics) {
            DrawNorm(prob,1);
            WaitTill(now+(time_seg*3));
        }
        if (DEBUG) ShowPath(prob->path);
    }
    newpl=PathLength(prob->path,prob->dist);
    if (graphics) {
        sprintf(buffer,"Lengths\n"
                   "Old = %.6f\n"
                   "New = %.6f\n",oldpl,newpl);
        ShowText(buffer,1,12,YELLOW);
    }
    return newpl;
}
