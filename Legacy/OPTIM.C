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

#define Reverse   1
#define NoReverse 0

double SingleChange(PROBLEM *p,INTARRAY *points)
// * adds up the cost of the point breaks * //
{
    int max=((points->size*2)/3)-1,x;
    double total=0.0;


    total+=p->dist->array[points->array[1]][points->array[2]];
    total+=p->dist->array[points->array[3]][points->array[4]];
    total+=p->dist->array[points->array[0]][points->array[5]];

    return total;
}

void SingleRevs(PROBLEM *p,INTARRAY *points,int revA,int revB,int revC)
{
    if (revA==Reverse) Swap(&(points->array[0]),&(points->array[1]));
    if (revB==Reverse) Swap(&(points->array[2]),&(points->array[3]));
    if (revC==Reverse) Swap(&(points->array[4]),&(points->array[5]));
}

double FindChange(PROBLEM *p,INTARRAY *points,int revA,int revB,int revC)
{
    double change=0.0;

    if ((revA+revB+revC)==0) return 0.0; // * no revs = no change * //
    else
    {
        change-=SingleChange(p,points);
        SingleRevs(p,points,revA,revB,revC); // apply //
        change+=SingleChange(p,points);
        SingleRevs(p,points,revA,revB,revC); // return to normal //
        return change;
    }
}

double FindBestRevs(PROBLEM *p,INTARRAY *points)
{
    int x,y,z;
    double bestchange=0.0,thischange;
    int at=(points->size*2)/3;

    points->array[6]=0;
    points->array[7]=0;
    points->array[8]=0;

    for (x=NoReverse;x<=Reverse;x++)
        for (y=NoReverse;y<=Reverse;y++)
            for (z=NoReverse;z<=Reverse;z++)
            {
                thischange=FindChange(p,points,x,y,z);
                if (thischange<bestchange)
                {
                    points->array[6]=x;
                    points->array[7]=y;
                    points->array[8]=z;
                    bestchange=thischange;
                }
            }
    return bestchange;
}

INTARRAY *NextKOpt(PROBLEM *p,int k)
{
    int elems=k*3;
    INTARRAY *result=NewIntArray(elems,0);     // * 3=p1,p2,rev * //
    INTARRAY *this=NewIntArray(elems,0);     // * 3=p1,p2,rev * //
    int *parray=PathToArray(p->path,1);
    int max=(p->path->size),x,y,z,pos,count;
    double bestval=0.0,thisval;

    char tmp[12];

    if (DEBUG) {
        for (x=0;x<=p->path->size;x++)
            printf("%d ",parray[x]);
        printf("\n");
    }
    
    
    for (x=0;x<max-2;x++)
    {
        pos=1;
        this->array[pos]=parray[x];
        this->array[pos+1]=parray[x+1];
        for (y=x+1;y<max-1;y++)
        {
            pos=3;
            this->array[pos]=parray[y];
            this->array[pos+1]=parray[y+1];

            for (z=y+1;z<max;z++) // * run through all possible split combos * //
            {
                pos=5;
                this->array[pos]=parray[z];
                this->array[0]=parray[z+1];

                thisval=FindBestRevs(p,this);
                if (thisval<bestval)
                {
                    CopyIntArray(result,this);
                    bestval=thisval;
                }
                


                // printf("%d,%d  %d,%d  %d,%d\n",this[0],this[1],
                //                                this[2],this[3],
                //                                this[4],this[5]);
                // SingleRevs(p,this,k,1,1,0);
                // 
                // printf("%d,%d  %d,%d  %d,%d\n",this[0],this[1],
                //                                this[2],this[3],
                //                                this[4],this[5]);



            }
        }
    }
    FreeIntArray(&this);
    sfree(parray);
    if (bestval==0.0) {FreeIntArray(&result); return NULL;}
    else return result;
}

void HiliteKOpt(PROBLEM *prob,INTARRAY *points,int lines)
{
    LOCATES *l=prob->dist->l;
    int x;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);

    for (x=0;x<(points->size*2)/3;x++)
        SetMaskAt(prob->vmask,city_hi,citysize,points->array[x]);

    DrawCities(l,prob->vmask);
    
    for (x=0;x<(points->size*2)/3;x++)
        SetMaskAt(prob->vmask,city_in,citysize,points->array[x]);

    BorderWin();
    SwapPage();
}


void MakeKOptTransition(PROBLEM *p,INTARRAY *points)
{
    if (points->array[6]==Reverse) {
        ReverseSubPath(p->path,points->array[5],points->array[0],
                               points->array[1],points->array[2]);
        Swap(&(points->array[0]),&(points->array[1]));
    }
    if (points->array[7]==Reverse) {
        ReverseSubPath(p->path,points->array[1],points->array[2],
                               points->array[3],points->array[4]);
        Swap(&(points->array[2]),&(points->array[3]));
    }
    if (points->array[8]==Reverse) {
        ReverseSubPath(p->path,points->array[3],points->array[4],
                               points->array[5],points->array[0]);
        Swap(&(points->array[4]),&(points->array[5]));
    }
}

double ControlKOpt(PROBLEM *p,int k,int graphics)
{
    INTARRAY *iarray=NextKOpt(p,k);
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/2;
    char buffer[50];

    if (graphics) ShowText("K-Opt Method",1,1,GREEN);

    while ((iarray->array[6]+iarray->array[7]+iarray->array[8])>0)
    {
        now=clock();
        if (graphics) {
            sprintf(buffer,"%2d-%2d %2d-%2d %2d-%2d\n%s   %s   %s\n",
                iarray->array[0],iarray->array[1],iarray->array[2],
                iarray->array[3],iarray->array[4],iarray->array[5],
                iarray->array[6]?"Rev":"No ",iarray->array[7]?"Rev":"No ",
                iarray->array[8]?"Rev":"No ");

            ShowText(buffer,1,12,YELLOW);
            HiliteKOpt(p,iarray,0);
            WaitTill(now+(time_seg));
        }
        if (DEBUG) {printf("%s",buffer);mygetch();}

        Working();
        MakeKOptTransition(p,iarray);
        if (graphics) {
            DrawNorm(p,1);
            WaitTill(now+(time_seg*2));
        }

        sfree(iarray);
        iarray=NextKOpt(p,k);
    }
    return PathLength(p->path,p->dist);
}
