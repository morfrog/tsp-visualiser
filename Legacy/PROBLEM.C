#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <conio.h>
#include <graph.h>
#include "sys.h"
#include "file.h"
#include "path.h"
#include "screen.h"
#include "problem.h"

int TotalDelay(int tdelay)
{
    static int tot_delay=-1; // * defaults to getkeypress option * //
    
    if (tdelay<0) return tot_delay;
    tot_delay=tdelay;
    return -1;
}

void WaitTill(clock_t it)
// * Waits until the clock() returns >= it * //
{
    if (TotalDelay(-1)<0) getch(); // * if Wait_For_Keypress option * //
    else
    {
        while (clock()<it)
        {
            if (kbhit()) {getch();break;}
        }
    }
}

void DrawCities(LOCATES *loc,MASK *m)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<(loc->num);count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
        DrawCity(num,loc->at[count].x,loc->at[count].y,m->array[count].size,m->array[count].color);
    }
}

void DrawPath(PATH *p,LOCATES *loc,int returnp)
{
    int count,a,b,max=p->size-(returnp?0:1);

    PathStart(p);
    for (count=0;count<max;count++)
    {
        PathPair(p,&a,&b);  // get the current path pair
        DrawLine(loc->at[a].x,loc->at[a].y,
                 loc->at[b].x,loc->at[b].y,
                 line_col);
        PathNext(p);
    }
}

PROBLEM *NewProblem(DISTANCES *dist)
{
    PROBLEM *ret=salloc(1,sizeof(PROBLEM));

    ret->dist=dist;

    ret->path=NULL;
    ret->vmask=NewViewMask(dist->size,city_out,citysize);

    ProblemReset(ret,0);
    return ret;
}

void ProblemReset(PROBLEM *prob,int free)
{
    SetMask(prob->vmask,city_out,citysize);    
    if (free) FreePath(prob->path);
}

double PathLength(PATH *p,DISTANCES *d)
{
    int count;
    double ret=0.0;
    NODE *look=p->head;

    if (p==NULL) return ret;

    for (count=0;count<p->size;count++)
    {
        ret+=d->array[look->citynum][look->next->citynum];
        look=look->next;
    }
    return ret;
}

double InsertCost(PROBLEM *prob,int I, int A, int B)
/* returns cost of inserting I between A and B */
{
    return prob->dist->array[A][I]+
            prob->dist->array[I][B]-
            prob->dist->array[B][A];
}

void BestInsert(PROBLEM *prob,int citynum,int *a,int *b)
{
    int pA,pB,x;
    double thiscost,bestcost;

    PathStart(prob->path);
    PathPair(prob->path,&pA,&pB);
    bestcost=InsertCost(prob,citynum,pA,pB);
    *a=pA;*b=pB;

    PathNext(prob->path);

    for (x=1;x<(prob->path->size);x++)
    {
        PathPair(prob->path,&pA,&pB);
        thiscost=InsertCost(prob,citynum,pA,pB);
        if (thiscost<bestcost)
        {
            bestcost=thiscost;
            *a=pA;*b=pB;
        }
        PathNext(prob->path);
    }
}

void HiliteCityGroup(PROBLEM *prob,int main,int maincol,int city1, int city2,int col)
{
    LOCATES *l=prob->dist->l;
    int A,B,C;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);
    A=SetMaskAt(prob->vmask,maincol,citysize*2,main);
    B=SetMaskAt(prob->vmask,col,citysize*2,city1);
    C=SetMaskAt(prob->vmask,col,citysize*2,city2);

    DrawCities(l,prob->vmask);

    SetMaskAt(prob->vmask,A,citysize,main);
    SetMaskAt(prob->vmask,B,citysize,city1);
    SetMaskAt(prob->vmask,C,citysize,city2);

    DrawLine(l->at[city1].x,l->at[city1].y,l->at[city2].x,l->at[city2].y,col);
    BorderWin();
    SwapPage();
}

void HiliteCityPair(PROBLEM *prob,int city1,int city2,int col)
{
    LOCATES *l=prob->dist->l;
    int A,B;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);

    A=SetMaskAt(prob->vmask,city_hi,citysize,city1);
    B=SetMaskAt(prob->vmask,city_hi,citysize,city2);
    DrawCities(l,prob->vmask);
    SetMaskAt(prob->vmask,A,citysize,city1);
    SetMaskAt(prob->vmask,B,citysize,city2);

    BorderWin();
    SwapPage();
}

void HiliteCity(PROBLEM *prob,int citynum,int returnp)
{
    LOCATES *l=prob->dist->l;
    int A;
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,returnp);

    A=SetMaskAt(prob->vmask,city_hi,citysize*2,citynum);
    DrawCities(l,prob->vmask);
    SetMaskAt(prob->vmask,A,citysize*2,citynum);
    BorderWin();
    SwapPage();
}

void DrawNorm(PROBLEM *prob,int returnp)
{
    LOCATES *l=prob->dist->l;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,returnp);
    DrawCities(l,prob->vmask);
    BorderWin();
    SwapPage();
}

MASK _FAR *NewViewMask(int size,int color,double csize)
{
    MASK *mask=NULL;

    mask=salloc(1,sizeof(MASK));
    mask->size=size;
    mask->array=salloc(mask->size,sizeof(VNODE));
    SetMask(mask,color,csize);
    return mask;
}

void _FAR SetMask(MASK *m,int color,double size)
{
    int x;
    for (x=0;x<m->size;x++) {
        m->array[x].color=color;
        m->array[x].size=size;
    }
}

void _FAR SetMaskCol(MASK *m,int color)
{
    int x;
    for (x=0;x<m->size;x++) m->array[x].color=color;
}

void _FAR SetMaskSize(MASK *m,double size)
{
    int x;
    for (x=0;x<m->size;x++) m->array[x].size=size;
}

int _FAR SetMaskAt(MASK *m,int color,double size,int pos)
{
    int ret=m->array[pos].color;
    if (color>-1)  m->array[pos].color=color;
    if (size>-1.0) m->array[pos].size=size;
    return ret;
}

void _FAR SetMasks(MASK *m,int num, ...) // int pos,int color,double size
{
    va_list ap;
    int x,pos,color;
    double size;

    va_start(ap,num);

    for (x=0;x<num;x++)
    {
        pos=va_arg(ap,int);
        color=va_arg(ap,int);
        size=va_arg(ap,double);
        m->array[pos].color=color;
        m->array[pos].size=size;
    }
    va_end(ap);
}

LOCATES *PathAsLocates(PATH *path,LOCATES *loc)
// * returns loc reordered in path order
// pre : path->size == loc->num
{
    LOCATES *ret=InitLocates(0,loc);
    int x,A;

    PathStart(path);
    for (x=0;x<path->size;x++) {
        PathCurrent(path,&A);
        ret->at[x].x=loc->at[A].x;
        ret->at[x].y=loc->at[A].y;
        ret->at[x].citynum=loc->at[A].citynum;
        PathNext(path);
    }
    return ret;
}

PATH *LocatesAsPath(LOCATES *loc)
{
    PATH *ret=NewPath(1,loc->at[0].citynum);
    int x;
    for (x=1;x<(loc->num);x++)
        InsertPathEnd(ret,loc->at[x].citynum);
    return ret;
}

/*
void main()
{
    PROBLEM *p=NewProblem(NewMap("30cities.da4"));
    int x,a,b;

    FStartPath(Lesser,p);          
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);
    x=FNextCost(Lesser,p,&a,&b);
    printf("Cheapest is %d, between %d and %d.\n",x,a,b);
    x=FNextCost(Greater,p,&a,&b);
    printf("Dearest is %d, between %d and %d.\n",x,a,b);

    ProblemReset(p,1);
    FStartPath(Greater,p);          
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);

    ProblemReset(p,1);
    RndStartPath(p);
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);
    FNextDist(Lesser,Lesser,p,&x);
    printf("-------\n");

    ProblemReset(p,1);
    getch();

    printf("Val  NN  FN\n");
    for (x=0;x<(p->endo);x++)
        printf("%3d %3d %3d\n",x,FNeighbour(Lesser,p,x),FNeighbour(Greater,p,x));
}
*/


