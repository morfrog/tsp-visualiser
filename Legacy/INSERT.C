#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <time.h>
#include "sys.h"
#include "file.h"
#include "screen.h"
#include "path.h"
#include "problem.h"
#include "insert.h"

INSPROB *NewInsProb(DISTANCES *dist)
{
    INSPROB *ret=salloc(1,sizeof(INSPROB));

    ret->prob=NewProblem(dist);
    ret->outpath=salloc(dist->size,sizeof(int));

    InsProbReset(ret,0);
    return ret;
}

void InsProbReset(INSPROB *insprob,int free)
{
    int x;
    
    insprob->endo=(insprob->prob->dist->size);
    for (x=0;x<(insprob->prob->dist->size);x++)
    {
        insprob->outpath[x]=x;
    }
    ProblemReset(insprob->prob,free);
}

void PrintInit(int init)
{
    switch (init) {
        case InitNearN   : printf("NearN    "); break;
        case InitRand    : printf("Random   "); break;
        case InitClosest : printf("Closest  "); break;
        case InitFurthest: printf("Furthest "); break;
        default :          printf("Illegal Value.");break; 
    }
}

void PrintMethod(int method)
{
    switch (method) {
        case InsRand    : printf("Random   "); break;
        case InsCheap   : printf("Cheapest "); break;
        case InsDear    : printf("Dearest  "); break;
        case InsNearN   : printf("NNeigbour"); break;
        case InsNearest : printf("Nearest  "); break;
        case InsFurthest: printf("Furthest "); break;
        default         : printf("Illegal Value."); break;
    }
}

void NNeighbourStartPath(INSPROB *insprob)
{
    int A,max=insprob->prob->dist->size;
    StartRandom();
    A=Random(max);
    CityDel(insprob,A);
    insprob->prob->path=NewPath(1,A);
    SetMaskAt(insprob->prob->vmask,city_in,citysize,A);
}

void RndStartPath(INSPROB *insprob)
{
    int A,B,max=insprob->prob->dist->size;

    StartRandom();
    A=Random(max);
    do B=Random(max); while (B==A);
    CityDel(insprob,A);
    CityDel(insprob,B);
    insprob->prob->path=NewPath(2,A,B);
    SetMaskAt(insprob->prob->vmask,city_in,citysize,A);
    SetMaskAt(insprob->prob->vmask,city_in,citysize,B);
}

void FStartPath(int (* func)(double,double),INSPROB *insprob)
{
    int A=0,B=1,x,y;
    int max=(insprob->prob->dist->size);
    double best=insprob->prob->dist->array[A][B];
    double thisdist;
    
    
    for (x=0;x<max;x++)
        for (y=0;y<max;y++)
            if (x!=y)
            {
                thisdist=insprob->prob->dist->array[x][y];
                if (func(thisdist,best))
                {
                    A=x;B=y;
                    best=thisdist;
                }
            }
    CityDel(insprob,A);
    CityDel(insprob,B);

    insprob->prob->path=NewPath(2,A,B);
    SetMaskAt(insprob->prob->vmask,city_in,citysize,A);
    SetMaskAt(insprob->prob->vmask,city_in,citysize,B);
}

void CityDel(INSPROB *insprob,int citynum)
/* deletes citynum from the problems outpath */
{
    int pos;
    pos=PosOf(citynum,insprob->outpath,(insprob->prob->dist->size));
    if (pos==-1) printf("%d Not In Out List.\n",citynum);
    insprob->endo-=1;
    insprob->outpath[pos]=insprob->outpath[insprob->endo];

}

int FNeighbour(int (* func)(double,double),INSPROB *insprob,int citynum)
/* returns the nearest/furthest neightbour of citynum in type path */
{
    int x,bestcity,thiscity;
    double bestdist,thisdist;

    if (insprob->outpath[0]==citynum)  // * func city to x is not x
        bestcity=insprob->outpath[1];
    else
        bestcity=insprob->outpath[0];

    bestdist=insprob->prob->dist->array[citynum][bestcity];

    for (x=0;x<(insprob->endo);x++)
    {
        thiscity=insprob->outpath[x];
        if (thiscity!=citynum)      // * func city to x is not x
        {
            thisdist=insprob->prob->dist->array[citynum][thiscity];
            if (func(thisdist,bestdist)) {
                bestdist=thisdist;
                bestcity=thiscity;
            }
        }
    }
    return bestcity;
}

int RndNext(INSPROB *insprob)
{
    return insprob->outpath[Random(insprob->endo)];
}

int FNextCost(int (* func)(double,double),INSPROB *insprob,int *A,int *B)
/* returns the cheapest/dearest for insertion of an outpath between a & b */
{
    int best;
    int listc,pathc;
    int pA,pB;
    double bestlen,thislen;

    best=insprob->outpath[0];

    PathStart(insprob->prob->path);
    PathPair(insprob->prob->path,A,B);
    bestlen=InsertCost(insprob->prob,best,*A,*B);

    for (listc=0;listc<insprob->endo;listc++)
    {
        for (pathc=0;pathc<(insprob->prob->path->size);pathc++)
        {
            PathPair(insprob->prob->path,&pA,&pB);
            thislen=InsertCost(insprob->prob,insprob->outpath[listc],pA,pB);
            if (func(thislen,bestlen))
            {
                best=insprob->outpath[listc];*A=pA;*B=pB;bestlen=thislen;
            }
            PathNext(insprob->prob->path);
        }
    }
    return best;
}

int FNextDist(int (* func)(double,double),int (* func2)(double,double),INSPROB *insprob,int *fcity)
/* returns the func of the func2 Nearest Neighbour, looking in outpath
 * e.g. FNextDist(Greater,Lesser,p);
 */
{
    int best,this;
    double bestlen,thislen;
    int x,city;

    PathStart(insprob->prob->path);
    PathCurrent(insprob->prob->path,&city);

    best=FNeighbour(*func2,insprob,city);
    bestlen=insprob->prob->dist->array[best][city];

    for (x=0;x<(insprob->prob->path->size);x++)
    {

        PathCurrent(insprob->prob->path,&city);
        this=FNeighbour(*func2,insprob,city);

        thislen=insprob->prob->dist->array[this][city];
        if (func(thislen,bestlen))
        {
            *fcity=city;
            best=this;
            bestlen=thislen;
        }
        PathNext(insprob->prob->path);
    }
    return best;
}

double ControlRand(INSPROB *insprob,int graphics)
{
    int next,Ia,Ib;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];
    static int col=WHITE;
    int total=insprob->endo;

    if (graphics) {sprintf(buffer,"Random Method\n");
                   ShowText(buffer,1,1,GREEN);}

    do {
        if (col==WHITE) col=LTWHITE; else col=WHITE;
        now=clock();
        next=RndNext(insprob);

        if (graphics) {sprintf(buffer,"Insert is %3d.  \n\n",next);
                       ShowText(buffer,1,4,col);}

        if (graphics) {HiliteCity(insprob->prob,next,1);WaitTill(now+time_seg);}

        CityDel(insprob,next);
        BestInsert(insprob->prob,next,&Ia,&Ib);

        if (graphics) {sprintf(buffer,"Between %3d and %3d.\n\n",Ia,Ib);
                       ShowText(buffer,1,6,col);}
        
        if (graphics) {HiliteCityGroup(insprob->prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(insprob->prob->path,next,Ia,Ib);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {sprintf(buffer,"%3d%% Complete.\n\n",100*(total-insprob->endo)/total);
                       ShowText(buffer,1,12,LTBLUE);}
        
        if (graphics) {DrawNorm(insprob->prob,1);WaitTill(now+(3*time_seg));}

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlNearN(INSPROB *insprob,int graphics)
{
    int x,next,this;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/2;
    char buffer[50];

    if (graphics) {sprintf(buffer,"Nearest Neighbour\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}

    PathEnd(insprob->prob->path);
    PathCurrent(insprob->prob->path,&this);

    do {
        now=clock();
        next=FNeighbour(Lesser,insprob,this);

        if (graphics) {sprintf(buffer,"Last Was %d.\n\nThis Is %d.",this,next);
                       ShowText(buffer,1,4,WHITE);}

        if (graphics) {HiliteCity(insprob->prob,next,0);WaitTill(now+time_seg);}

        CityDel(insprob,next);
        InsertPathEnd(insprob->prob->path,next);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);
        
        if (graphics) {DrawNorm(insprob->prob,0);WaitTill(now+(2*time_seg));}
        this=next;

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlFurthest(INSPROB *insprob,int graphics)
{
    int next,Ia,Ib,it;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];

    if (graphics) {sprintf(buffer,"    Furthest\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}
    

    do {
        now=clock();
        next=FNextDist(Greater,Lesser,insprob,&it);

        if (graphics) {sprintf(buffer,"Last Was %d.\n\nThis Is %d.",it,next);
                       ShowText(buffer,1,4,WHITE);}
        
        if (graphics) {HiliteCity(insprob->prob,next,1);WaitTill(now+time_seg);}

        CityDel(insprob,next);
        BestInsert(insprob->prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(insprob->prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(insprob->prob->path,next,Ia,Ib);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(insprob->prob,1);WaitTill(now+(3*time_seg));}

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlNearest(INSPROB *insprob,int graphics)
{
    int next,Ia,Ib,it;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];

    if (graphics) {sprintf(buffer,"    Nearest\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}

    do {
        now=clock();
        next=FNextDist(Lesser,Lesser,insprob,&it);

        if (graphics) {HiliteCity(insprob->prob,next,1);WaitTill(now+time_seg);}

        CityDel(insprob,next);
        BestInsert(insprob->prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(insprob->prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(insprob->prob->path,next,Ia,Ib);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(insprob->prob,1);WaitTill(now+(3*time_seg));}

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlDear(INSPROB *insprob,int graphics)
{
    int next,pA,pB;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];

    if (graphics) {sprintf(buffer,"    Dearest\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}


    do {
        now=clock();
        next=FNextCost(Greater,insprob,&pA,&pB);
        BestInsert(insprob->prob,next,&pA,&pB);
        if (graphics) {HiliteCity(insprob->prob,next,1);WaitTill(now+(time_seg));}
        if (graphics) {HiliteCityGroup(insprob->prob,next,YELLOW,pA,pB,BLUE);WaitTill(now+(2*time_seg));}
        CityDel(insprob,next);
        InsertPath(insprob->prob->path,next,pA,pB);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(insprob->prob,1);WaitTill(now+(3*time_seg));}

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlCheap(INSPROB *insprob,int graphics)
{
    int next,pA,pB;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];

    if (graphics) {sprintf(buffer,"    Cheapest\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}


    do {
        now=clock();
        next=FNextCost(Lesser,insprob,&pA,&pB);

        if (graphics) {HiliteCity(insprob->prob,next,1);WaitTill(now+(time_seg));}
        if (graphics) {HiliteCityGroup(insprob->prob,next,YELLOW,pA,pB,BLUE);WaitTill(now+(2*time_seg));}
        CityDel(insprob,next);
        InsertPath(insprob->prob->path,next,pA,pB);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(insprob->prob,1);WaitTill(now+(3*time_seg));}

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

