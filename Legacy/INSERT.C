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

double InsertCost(PROBLEM *prob,int I, int A, int B)
/* returns cost of inserting I between A and B */
{
    return prob->dist->array[A][I]+
            prob->dist->array[I][B]-
            prob->dist->array[B][A];
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

double ControlRand(PROBLEM *prob,int graphics)
{
    int next,Ia,Ib;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[50];
    static int col=WHITE;
    int total=prob->endo;

    if (graphics) {sprintf(buffer,"Random Method\n");
                   ShowText(buffer,1,1,GREEN);}

    do {
        if (col==WHITE) col=LTWHITE; else col=WHITE;
        now=clock();
        next=RndNext(prob);

        if (graphics) {sprintf(buffer,"Insert is %3d.  \n\n",next);
                       ShowText(buffer,1,4,col);}

        if (graphics) {HiliteCity(prob,next,1);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {sprintf(buffer,"Between %3d and %3d.\n\n",Ia,Ib);
                       ShowText(buffer,1,6,col);}
        
        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);
        SetMaskAt(prob->vmask,city_in,citysize,next);

        if (graphics) {sprintf(buffer,"%3d%% Complete.\n\n",100*(total-prob->endo)/total);
                       ShowText(buffer,1,12,LTBLUE);}
        
        if (graphics) {DrawNorm(prob,1);WaitTill(now+(3*time_seg));}

    } while (prob->endo);

    if (graphics) {sprintf(buffer,"F I N I S H E D");
                   ShowText(buffer,1,16,LTRED);}

    return PathLength(prob->path,prob->dist);
}

double ControlNearN(PROBLEM *prob,int graphics)
{
    int x,next,this;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/2;
    char buffer[50];

    if (graphics) {sprintf(buffer,"Nearest Neighbour\n"
                                  "     Method\n");
                   ShowText(buffer,1,1,GREEN);}

    PathEnd(prob->path);
    PathCurrent(prob->path,&this);

    do {
        now=clock();
        next=FNeighbour(Lesser,prob,this);

        if (graphics) {sprintf(buffer,"Last Was %d.\n\nThis Is %d.",this,next);
                       ShowText(buffer,1,4,WHITE);}

        if (graphics) {HiliteCity(prob,next,0);WaitTill(now+time_seg);}

        CityDel(prob,next);
        InsertPathEnd(prob->path,next);
        SetMaskAt(prob->vmask,city_in,citysize,next);
        
        if (graphics) {DrawNorm(prob,0);WaitTill(now+(2*time_seg));}
        this=next;

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlFurthest(PROBLEM *prob,int graphics)
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
        next=FNextDist(Greater,Lesser,prob,&it);

        if (graphics) {sprintf(buffer,"Last Was %d.\n\nThis Is %d.",it,next);
                       ShowText(buffer,1,4,WHITE);}
        
        if (graphics) {HiliteCity(prob,next,1);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);
        SetMaskAt(prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(prob,1);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlNearest(PROBLEM *prob,int graphics)
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
        next=FNextDist(Lesser,Lesser,prob,&it);

        if (graphics) {HiliteCity(prob,next,1);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);
        SetMaskAt(prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(prob,1);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlDear(PROBLEM *prob,int graphics)
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
        next=FNextCost(Greater,prob,&pA,&pB);
        BestInsert(prob,next,&pA,&pB);
        if (graphics) {HiliteCity(prob,next,1);WaitTill(now+(time_seg));}
        if (graphics) {HiliteCityGroup(prob,next,YELLOW,pA,pB,BLUE);WaitTill(now+(2*time_seg));}
        CityDel(prob,next);
        InsertPath(prob->path,next,pA,pB);
        SetMaskAt(prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(prob,1);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlCheap(PROBLEM *prob,int graphics)
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
        next=FNextCost(Lesser,prob,&pA,&pB);

        if (graphics) {HiliteCity(prob,next,1);WaitTill(now+(time_seg));}
        if (graphics) {HiliteCityGroup(prob,next,YELLOW,pA,pB,BLUE);WaitTill(now+(2*time_seg));}
        CityDel(prob,next);
        InsertPath(prob->path,next,pA,pB);
        SetMaskAt(prob->vmask,city_in,citysize,next);

        if (graphics) {DrawNorm(prob,1);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

