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

// #define DEBUG 0

char *OptStr(int method)
{
    char *buffer=salloc(20,sizeof(char));
    switch (method) {
        case opt1   : sprintf(buffer,"One Opt\nMethod");    break;
        case opt2   : sprintf(buffer,"Two Opt\nMethod");    break;
        case opt3   : sprintf(buffer,"Three Opt\nMethod");  break;
        default     : buffer=NULL;                          break; 
    }
    return buffer;
}

double ControlAllOpt(PROBLEM *prob,int method,int graphics,int tdelay)
{
    if ((graphics) || (tdelay==0)) OptShowText(OptStr(method),1,1,GREEN);

    switch (method) {
        case opt1 : return ControlOneOpt(prob,graphics);break;
        case opt2 : return ControlTwoOpt(prob,graphics);break;
        case opt3 : return Control3Opt(prob,graphics);  break;
        default : printf("Invalid Method");exit(0);     break;
    }
    return 0.0;
}

int NextOneOpt(PROBLEM *prob,int *next,int *oA,int *oB,int *nA,int *nB,double *improve)
// * returns 1 if successfully finds one, 0 otherwise * //
// * sets next, A & B being place to insert next      * //
{

    int *path=PathToArray(prob->path,0); /* list of all to check */
    int max=prob->path->size;
    int insert,A,B,C,D,I;
    double removal,insertion,thisimp;

    *improve=0.0;

    for (insert=0;insert<max;insert++) // * run thru all cities * //
    {
            I=path[insert];
            TmpPathDel(prob->path,I,&A,&B);
#ifdef DEBUG
    printf("Checking %d from %d->%d\n",I,A,B);
#endif

            removal=(prob->dist->array[A][B])
                   -(prob->dist->array[I][A])
                   -(prob->dist->array[I][B]);
#ifdef DEBUG
    printf("Removal is %.4f\n",removal);
#endif
            
            BestInsert(prob,I,&C,&D);
            insertion=InsertCost(prob,I,C,D);

#ifdef DEBUG
    printf("Insertion between %d,%d is %.4f\n",C,D,insertion);
    printf("This %.4f Best %.4f\n",thisimp,improve);
#endif
            thisimp=insertion+removal;
            if ((thisimp<*improve) && (!(Same(A,B,C,D))) )
            {
                *improve=thisimp;
                *next=I;
                *oA=A;
                *oB=B;
                *nA=C;
                *nB=D;
                
            }
            TmpPathIns(prob->path);
    }
    if (*improve>=0.0) return 0;
    return 1;
}

void HiliteOneOpt(PROBLEM *prob,int A,int B,int C,int color,int lines)
{
    LOCATES *l=prob->dist->l;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);

    if (lines) {
        DrawCitiesLine(l,A,B,color);
        DrawCitiesLine(l,B,C,color);
    }
    SetMasks(prob->vmask,3,A,color,citysize,
                           B,color,citysize*2,
                           C,color,citysize);
    
    DrawCities(l,prob->vmask);
    
    SetMasks(prob->vmask,3,A,city_in,citysize,
                           B,city_in,citysize,
                           C,city_in,citysize);

}

double ControlOneOpt(PROBLEM *prob,int graphics)
{
    int next,oldA,oldB,newA,newB,times=0;
    char buffer[50];
    clock_t now;
    clock_t seg1,seg2,seg3;
    double improve;

    Init3TimeSegs(&seg1,&seg2,&seg3);
    if (graphics) {
        sprintf(buffer,"One Opt Method\n");
        ShowText(buffer,1,1,GREEN);
    }

    while (NextOneOpt(prob,&next,&oldA,&oldB,&newA,&newB,&improve))
    {

        now=clock();

        if (graphics) {
            sprintf(buffer,"Run %d\n",times);
            ShowText(buffer,1,3,YELLOW);
            ++times;
            sprintf(buffer,"%2d from %2d,%2d\n"
                           "                \n",next,oldA,oldB);
            ShowText(buffer,1,4,hilite_1);
            sprintf(buffer,"                   \n"
                           "                   \n");
            ShowText(buffer,1,6,BLACK);
            HiliteOneOpt(prob,oldA,next,oldB,hilite_1,1);
            SwapPage();
            if (WaitTill(now+seg1)) Init3TimeSegs(&seg1,&seg2,&seg3);
    
            sprintf(buffer,"%2d to %2d,%2d  ",next,newA,newB);
            ShowText(buffer,1,5,hilite_2);
            HiliteOneOpt(prob,newA,next,newB,hilite_2,0);
            SwapPage();
            if (WaitTill(now+seg2)) Init3TimeSegs(&seg1,&seg2,&seg3);
        }

        DeletePath(prob->path,next);
        InsertPath(prob->path,next,newA,newB);

        if (graphics) {
            DrawNorm(prob,1);
            SwapPage();
            sprintf(buffer,"Path Length\n"
                           "Decrease = %8.6f",-improve);
            ShowText(buffer,1,6,YELLOW);
            if (WaitTill(now+seg3)) Init3TimeSegs(&seg1,&seg2,&seg3);
        }
    }
    return PathLength(prob->path,prob->dist);
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
#ifdef DEBUG
    printf("%d %d | %d %d\n",mA,mB,mC,mD);
#endif

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
    int elems,swaps,x;

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
#ifdef DEBUG
    printf("%d,%d,%d,%d ",A,B,C,D);ShowPath(p->path);
#endif

    ReverseSubPath(p->path,A,B,C,D);
   
}

void HiliteTwoOpt(PROBLEM *prob,int A,int B,int C,int D,int type)
{
    LOCATES *l=prob->dist->l;
    int col=hilite_1,x,this;

    _clearscreen(_GVIEWPORT);

    PathStart(prob->path);
    for (x=0;x<prob->path->size;x++)
    {
        PathCurrent(prob->path,&this);
        if (this==B)
            col=hilite_2;
        else if (this==D)
            col=hilite_1;
        SetMaskAt(prob->vmask,col,citysize,this);
        PathNext(prob->path);
    }
    DrawMaskPath(prob->path,l,prob->vmask);
    if (type!=0)
    {
        DrawCitiesLine(l,A,B,BLACK);
        DrawCitiesLine(l,C,D,BLACK);
        DrawCitiesLine(l,A,C,line_col);
        DrawCitiesLine(l,B,D,line_col);
    }
    DrawCities(l,prob->vmask);
    SetMask(prob->vmask,city_out,citysize);
}

double ControlTwoOpt(PROBLEM *prob,int graphics)
{
    int A,B,C,D;
    char buffer[60];
    double dab,dcd,dac,dbd;
    clock_t now;
    clock_t seg1,seg2,seg3;

    Init3TimeSegs(&seg1,&seg2,&seg3);

#ifdef DEBUG
    ShowPath(prob->path);
#endif

    if (graphics) {
        sprintf(buffer,"Two Opt Method\n");
        ShowText(buffer,1,1,GREEN);
    }
    while (NextTwoOpt(prob,&A,&B,&C,&D))
    {
        now=clock();
#ifdef DEBUG
    printf("Next is %d %d to %d %d\n",A,B,C,D);
#endif
        if (graphics) {
            HiliteTwoOpt(prob,A,B,C,D,0);
            SwapPage();
            sprintf(buffer,"Paths %2d->%2d\n",B,C);
            ShowText(buffer,1,3,hilite_2);
            sprintf(buffer,"And   %2d->%2d\n",A,D);
            ShowText(buffer,1,4,hilite_1);                           

            dab=prob->dist->array[A][B];
            dcd=prob->dist->array[C][D];
            dac=prob->dist->array[A][C];
            dbd=prob->dist->array[B][D];
            sprintf(buffer,"Breaking\n"
                           "%2d->%2d = %8.6f\n"
                           "%2d->%2d = %8.6f\n",
                           A,B,dab,
                           C,D,dcd);
            ShowText(buffer,1,5,GREEN);

            sprintf(buffer,"Adding\n"
                           "%2d->%2d = %8.6f\n"
                           "%2d->%2d = %8.6f\n",
                           A,C,dac,
                           B,D,dbd);
            ShowText(buffer,1,8,GREEN);
            sprintf(buffer,"Path Length\n"
                           "Decrease = %8.6f\n",0.0-(dac-dab+dbd-dcd));
            ShowText(buffer,1,11,YELLOW);
            if (WaitTill(now+seg1)) Init3TimeSegs(&seg1,&seg2,&seg3);
            
            HiliteTwoOpt(prob,A,B,C,D,1);
            SwapPage();
            if (WaitTill(now+seg2)) Init3TimeSegs(&seg1,&seg2,&seg3);

        }
        MakeTwoOptTransition(prob,A,B,C,D);
        if (graphics) {
            DrawNorm(prob,1);
            SwapPage();
            if (WaitTill(now+seg3)) Init3TimeSegs(&seg1,&seg2,&seg3);
        }
#ifdef DEBUG
    ShowPath(prob->path);
#endif
    }
    
    return PathLength(prob->path,prob->dist);
}

#define Reverse   1
#define NoReverse 0

double SingleChange(PROBLEM *p,INTARRAY *points)
// * adds up the cost of the point breaks * //
{
    double total=0.0;

    total+=p->dist->array[points->array[1]][points->array[2]];
    total+=p->dist->array[points->array[3]][points->array[4]];
    total+=p->dist->array[points->array[0]][points->array[5]];

    return total;
}

void SingleRevs(INTARRAY *points,int revA,int revB,int revC)
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
        SingleRevs(points,revA,revB,revC); // apply //
        change+=SingleChange(p,points);
        SingleRevs(points,revA,revB,revC); // return to normal //
        return change;
    }
}

double FindBestRevs(PROBLEM *p,INTARRAY *points)
{
    int x,y,z;
    double bestchange=0.0,thischange;

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

INTARRAY *Next3Opt(PROBLEM *p)
{
    int elems=9;
    INTARRAY *result=NewIntArray(elems,0);     // * 3=p1,p2,rev * //
    INTARRAY *this=NewIntArray(elems,0);     // * 3=p1,p2,rev * //
    int *parray=PathToArray(p->path,1);
    int max=(p->path->size),x,y,z,pos;
    double bestval=0.0,thisval;

#ifdef DEBUG
        for (x=0;x<=p->path->size;x++)
            printf("%d ",parray[x]);
        printf("\n");
#endif
    
    
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
                // SingleRevs(this,k,1,1,0);
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

void Hilite3Opt(PROBLEM *prob,INTARRAY *points,int lines)
{
    LOCATES *l=prob->dist->l;
    int col=hilite_1,x,this;

    _clearscreen(_GVIEWPORT);

    PathStart(prob->path);

    while (PathCurrent(prob->path,&this)!=points->array[0]) PathNext(prob->path);

    for (x=0;x<prob->path->size;x++)
    {
        PathCurrent(prob->path,&this);
        if (this==points->array[2])
            col=hilite_2;
        else if (this==points->array[4])
            col=hilite_3;
        SetMaskAt(prob->vmask,col,citysize,this);
        PathNext(prob->path);
    }
    DrawMaskPath(prob->path,l,prob->vmask);
    DrawCities(l,prob->vmask);
    SetMask(prob->vmask,city_out,citysize);

    SwapPage();

}


void Make3OptTransition(PROBLEM *p,INTARRAY *points)
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

double Control3Opt(PROBLEM *p,int graphics)
{
    INTARRAY *iarray=Next3Opt(p);
    clock_t now;
    clock_t seg1,seg2,seg3;
    char buffer[50];

    Init3TimeSegs(&seg1,&seg2,&seg3);

    if (graphics) ShowText("Three Opt Method",1,1,GREEN);

    while ((iarray->array[6]+iarray->array[7]+iarray->array[8])>0)
    {
        now=clock();
        if (graphics) {
            sprintf(buffer,"%2d->%2d %s\n",
                iarray->array[0],iarray->array[1],iarray->array[6]?"Rev":"No ");
            ShowText(buffer,1,5,hilite_1);
            sprintf(buffer,"%2d->%2d %s\n",
                iarray->array[2],iarray->array[3],iarray->array[7]?"Rev":"No ");
            ShowText(buffer,1,6,hilite_2);
            sprintf(buffer,"%2d->%2d %s\n",
                iarray->array[4],iarray->array[5],iarray->array[8]?"Rev":"No ");
            ShowText(buffer,1,7,hilite_3);

            Hilite3Opt(p,iarray,0);
            if (WaitTill(now+seg1)) Init3TimeSegs(&seg1,&seg2,&seg3);
        }
#ifdef DEBUG
    printf("%s",buffer);
    mygetch();
#endif

        Make3OptTransition(p,iarray);
        if (graphics) {
            Hilite3Opt(p,iarray,0);
            if (WaitTill(now+seg2)) Init3TimeSegs(&seg1,&seg2,&seg3);

            DrawNorm(p,1);
            SwapPage();
            if (WaitTill(now+seg3)) Init3TimeSegs(&seg1,&seg2,&seg3);
        }

        sfree(iarray);
        iarray=Next3Opt(p);
    }
    return PathLength(p->path,p->dist);
}

void _FAR OptWinManager(int which)
{
    static int init=0;
    static SWIN *main;
    static SWIN *text;

    if (!init) {
        main=NewWin(1,1,460,349);
        text=NewWin(465,1,639,349);
        BorderWin(main,border_col);
        BorderWin(text,border_col);
        init=1;
    }
    switch (which) {
        case Main : SetWin(main);  break;
        case Text : SetTextWin(text);  break;
    }
}

void _FAR OptShowText(char string[],int start,int line,int color)
{
    int startpage=_getactivepage();
    _setactivepage(1);
    DrawText(string,start,line,color);
    _setactivepage(0);
    DrawText(string,start,line,color);
    _setactivepage(startpage);
}
