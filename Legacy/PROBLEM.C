#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <conio.h>
#include "sys.h"
#include "file.h"
#include "path.h"
#include "screen.h"
#include "problem.h"

PROBLEM *NewProblem(DISTANCES *dist)
{
    PROBLEM *ret=salloc(1,sizeof(PROBLEM));

    ret->dist=dist;

    ret->outpath=salloc(dist->size,sizeof(int));
    ret->path=NULL;
    ret->vmask=NewViewMask(dist->size,city_out,citysize);

    ProblemReset(ret,0);
    return ret;
}

void ProblemReset(PROBLEM *prob,int free)
{
    int x;
    
    prob->endo=(prob->dist->size);
    for (x=0;x<(prob->dist->size);x++)
    {
        prob->outpath[x]=x;
    }
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
    return  prob->dist->array[A][I]+
            prob->dist->array[I][B]-
            prob->dist->array[B][A];
}

void PrintList(int *list,int listsize)
{
    int x;
    for (x=0;x<listsize;x++)
        printf("%2d ",list[x]);
    printf("\n");
}

int PosOf(int num,int *list,int listsize)
/* returns pos of num in unsorted list, -1 of not in list */
{
    int x;
    for (x=0;x<listsize;x++)
        if (list[x]==num) return x;
    return -1;
}

void CityDel(PROBLEM *prob,int citynum)
/* deletes citynum from the problems outpath */
{
    int pos;
    pos=PosOf(citynum,prob->outpath,(prob->dist->size));
    if (pos==-1) printf("%d Not In Out List.\n",citynum);
    prob->endo-=1;
    prob->outpath[pos]=prob->outpath[prob->endo];

}

int Lesser(double this,double best)
{
    if (this<best) return 1;
    return 0;
}

int Greater(double this,double best)
{
    if (this>best) return 1;
    return 0;
}

void NNeighbourStartPath(PROBLEM *prob)
{
    int A,max=prob->dist->size;
    StartRandom();
    A=Random(max);
    CityDel(prob,A);
    prob->path=NewPath(1,A);
    SetMaskAt(prob->vmask,city_in,citysize,A);
}

void RndStartPath(PROBLEM *prob)
{
    int A,B,C,max=prob->dist->size;

    StartRandom();
    A=Random(max);
    do B=Random(max); while (B==A);
    do C=Random(max); while ((C==B) || (C==A));
    CityDel(prob,A);
    CityDel(prob,B);
    CityDel(prob,C);
    prob->path=NewPath(3,A,B,C);
    SetMaskAt(prob->vmask,city_in,citysize,A);
    SetMaskAt(prob->vmask,city_in,citysize,B);
    SetMaskAt(prob->vmask,city_in,citysize,C);
}

void FStartPath(int (* func)(double,double),PROBLEM *prob)
{
    int A=0,B=1,C=2,x,y,z;
    int max=(prob->dist->size);
    double best=prob->dist->array[A][B]
                +prob->dist->array[B][C]
                +prob->dist->array[C][A];
    double thisdist;
    
    
    for (x=0;x<max;x++)
        for (y=0;y<max;y++)
            if (x!=y)
                for (z=0;z<max;z++)
                    if ((z!=y) && (z!=x))
                    {
                        thisdist=prob->dist->array[x][y]
                                +prob->dist->array[y][z]
                                +prob->dist->array[z][x];
                        if (func(thisdist,best))
                        {
                            A=x;B=y;C=z;
                            best=thisdist;
                        }
                    }
    CityDel(prob,A);
    CityDel(prob,B);
    CityDel(prob,C);
    prob->path=NewPath(3,A,B,C);
    SetMaskAt(prob->vmask,city_in,citysize,A);
    SetMaskAt(prob->vmask,city_in,citysize,B);
    SetMaskAt(prob->vmask,city_in,citysize,C);

}

int FNeighbour(int (* func)(double,double),PROBLEM *prob,int citynum)
/* returns the nearest/furthest neightbour of citynum in type path */
{
    int x,bestcity,thiscity;
    double bestdist,thisdist;

    if (prob->outpath[0]==citynum)  // * func city to x is not x
        bestcity=prob->outpath[1];
    else
        bestcity=prob->outpath[0];

    bestdist=prob->dist->array[citynum][bestcity];

    for (x=0;x<(prob->endo);x++)
    {
        thiscity=prob->outpath[x];
        if (thiscity!=citynum)      // * func city to x is not x
        {
            thisdist=prob->dist->array[citynum][thiscity];
            if (func(thisdist,bestdist)) {
                bestdist=thisdist;
                bestcity=thiscity;
            }
        }
    }
    return bestcity;
}

int RndNext(PROBLEM *prob)
{
    return prob->outpath[Random(prob->endo)];
}

int FNextCost(int (* func)(double,double),PROBLEM *prob,int *A,int *B)
/* returns the cheapest/dearest for insertion of an outpath between a & b */
{
    int best;
    int listc,pathc;
    int pA,pB;
    double bestlen,thislen;

    best=prob->outpath[0];

    PathStart(prob->path);
    PathPair(prob->path,A,B);
    bestlen=InsertCost(prob,best,*A,*B);

    for (listc=0;listc<prob->endo;listc++)
    {
        for (pathc=0;pathc<(prob->path->size);pathc++)
        {
            PathPair(prob->path,&pA,&pB);
            thislen=InsertCost(prob,prob->outpath[listc],pA,pB);
            if (func(thislen,bestlen))
            {
                best=prob->outpath[listc];*A=pA;*B=pB;bestlen=thislen;
            }
            PathNext(prob->path);
        }
    }
    return best;
}

int FNextDist(int (* func)(double,double),int (* func2)(double,double),PROBLEM *prob,int *fcity)
/* returns the func of the func2 Nearest Neighbour, looking in outpath
 * e.g. FNextDist(Greater,Lesser,p);
 */
{
    int best,this;
    double bestlen,thislen;
    int x,city;

    PathStart(prob->path);
    PathCurrent(prob->path,&city);

    best=FNeighbour(*func2,prob,city);
    bestlen=prob->dist->array[best][city];

    for (x=0;x<(prob->path->size);x++)
    {

        PathCurrent(prob->path,&city);
        this=FNeighbour(*func2,prob,city);

        thislen=prob->dist->array[this][city];
        if (func(thislen,bestlen))
        {
            *fcity=city;
            best=this;
            bestlen=thislen;
        }
        PathNext(prob->path);
    }
    return best;
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


