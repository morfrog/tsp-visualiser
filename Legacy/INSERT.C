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

char *InitStr(int init)
{
    char *buffer=salloc(20,sizeof(char));
    switch (init) {
        case InitNearN   : sprintf(buffer,"Nearest Neighbour"); break;
        case InitRand    : sprintf(buffer,"Random");            break;
        case InitClosest : sprintf(buffer,"Closest");           break;
        case InitFurthest: sprintf(buffer,"Furthest");          break;
        default :          buffer=NULL;                         break; 
    }
    return buffer;
}

char *MethodStr(int method)
{
    char *buffer=salloc(20,sizeof(char));
    switch (method) {
        case InsRand    : sprintf(buffer,"Random");             break;
        case InsCheap   : sprintf(buffer,"Cheapest");           break;
        case InsDear    : sprintf(buffer,"Dearest");            break;
        case InsNearN   : sprintf(buffer,"Nearest Neigbour");   break;
        case InsNearest : sprintf(buffer,"Nearest");            break;
        case InsFurthest: sprintf(buffer,"Furthest");           break;
        default         : buffer=NULL;                          break; 
    }
    return buffer;
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

void ShowChanges(INSPROB *insprob,int next,int Ia,int Ib,int color,int line)
{
    char buffer[80];
    sprintf(buffer,"Between %3d and %3d.\n",Ia,Ib);
    ShowText(buffer,1,line,color);
    sprintf(buffer,"Edge Deleted =\n"
                   "%3d->%3d is %6.5f\n",Ia,Ib,
                   insprob->prob->dist->array[Ia][Ib]);
    ShowText(buffer,1,line+1,color);
    sprintf(buffer,"Edges Added = \n"
                   "%3d->%3d is %6.5f\n"
                   "%3d->%3d is %6.5f\n",
                   next,Ia,insprob->prob->dist->array[next][Ia],
                   next,Ib,insprob->prob->dist->array[next][Ib]);
    ShowText(buffer,1,line+4,color);
    sprintf(buffer,"Increase To Path =\n"
                   "%6.5f",InsertCost(insprob->prob,next,Ia,Ib));
    ShowText(buffer,1,line+8,color);
}

double ControlRand(INSPROB *insprob,int graphics)
{
    int next,Ia,Ib;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    char buffer[80];
    static int col=WHITE;
    int total=insprob->endo;

    do {
        if (col==WHITE) col=LTWHITE; else col=WHITE;
        now=clock();
        next=RndNext(insprob);

        if (graphics) {
            sprintf(buffer,"Insert is %3d.  \n\n",next);
            ShowText(buffer,1,4,col);
            HiliteCity(insprob->prob,next,1);
            WaitTill(now+time_seg);
        }

        CityDel(insprob,next);
        BestInsert(insprob->prob,next,&Ia,&Ib);

        if (graphics) {
            ShowChanges(insprob,next,Ia,Ib,col,5);
            HiliteCityGroup(insprob->prob,next,YELLOW,Ia,Ib,BLUE);
            WaitTill(now+(2*time_seg));
        }

        InsertPath(insprob->prob->path,next,Ia,Ib);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {
            sprintf(buffer,"%3d%% Complete.\n\n",100*(total-insprob->endo)/total);
            ShowText(buffer,1,16,LTBLUE);
            DrawNorm(insprob->prob,1);
            WaitTill(now+(3*time_seg));
        }

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlNearN(INSPROB *insprob,int graphics)
{
    int x,next,this;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/2;
    char buffer[50];

    PathEnd(insprob->prob->path);
    PathCurrent(insprob->prob->path,&this);

    do {
        now=clock();
        next=FNeighbour(Lesser,insprob,this);

        if (graphics) {
            sprintf(buffer,"Last Was %d.\n\nThis Is %d.",this,next);
            ShowText(buffer,1,4,WHITE);
            HiliteCity(insprob->prob,next,0);
            WaitTill(now+time_seg);
        }

        CityDel(insprob,next);
        InsertPathEnd(insprob->prob->path,next);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);
        
        if (graphics) {
            DrawNorm(insprob->prob,0);
            WaitTill(now+(2*time_seg));
        }
        this=next;

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlDist(INSPROB *insprob,int (* func)(double,double),int (* func2)(double,double),int graphics)
{
    int next,Ia,Ib,it;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    static int col=WHITE;
    char buffer[50];

    do {
        now=clock();
        if (col==WHITE) col=LTWHITE; else col=WHITE;

        next=FNextDist(*func,*func2,insprob,&it);

        if (graphics) {
            sprintf(buffer,"Insert is %3d.  \n\n",next);
            ShowText(buffer,1,4,col);
            HiliteCity(insprob->prob,next,1);
            WaitTill(now+time_seg);
        }

        CityDel(insprob,next);
        BestInsert(insprob->prob,next,&Ia,&Ib);

        if (graphics) {
            ShowChanges(insprob,next,Ia,Ib,col,5);
            HiliteCityGroup(insprob->prob,next,YELLOW,Ia,Ib,BLUE);
            WaitTill(now+(2*time_seg));
        }

        InsertPath(insprob->prob->path,next,Ia,Ib);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {
            DrawNorm(insprob->prob,1);
            WaitTill(now+(3*time_seg));
        }

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);


}

double ControlFurthest(INSPROB *insprob,int graphics)
{
    return ControlDist(insprob,Greater,Lesser,graphics);
}

double ControlNearest(INSPROB *insprob,int graphics)
{
    return ControlDist(insprob,Lesser,Lesser,graphics);
}

double ControlCost(INSPROB *insprob,int (* func)(double,double),int graphics)
{
    int next,pA,pB;
    clock_t now;
    clock_t time_seg=TotalDelay(-1)/3;
    static int col=WHITE;
    char buffer[50];

    do {
        now=clock();
        if (col==WHITE) col=LTWHITE; else col=WHITE;

        next=FNextCost(*func,insprob,&pA,&pB);
        BestInsert(insprob->prob,next,&pA,&pB);

        if (graphics) {
            HiliteCity(insprob->prob,next,1);
            sprintf(buffer,"Insert is %3d.  \n\n",next);
            ShowText(buffer,1,4,col);
            WaitTill(now+(time_seg));
            HiliteCityGroup(insprob->prob,next,YELLOW,pA,pB,BLUE);
            ShowChanges(insprob,next,pA,pB,col,5);
            WaitTill(now+(2*time_seg));
        }

        CityDel(insprob,next);
        InsertPath(insprob->prob->path,next,pA,pB);
        SetMaskAt(insprob->prob->vmask,city_in,citysize,next);

        if (graphics) {
            DrawNorm(insprob->prob,1);
            WaitTill(now+(3*time_seg));
        }

    } while (insprob->endo);

    return PathLength(insprob->prob->path,insprob->prob->dist);
}

double ControlDear(INSPROB *insprob,int graphics)
{
    return ControlCost(insprob,Greater,graphics);
}

double ControlCheap(INSPROB *insprob,int graphics)
{
    return ControlCost(insprob,Lesser,graphics);
}

double ControlAllIns(INSPROB *insprob,int init,int method,int graphics)
{

    char buffer[40];
    if (graphics)
    {
        sprintf(buffer,"%s\nMethod\n",MethodStr(method));
        ShowText(buffer,1,1,GREEN);
    }

    if (method==InsNearN)
    {
        NNeighbourStartPath(insprob);
        return ControlNearN(insprob,graphics);
    }
    else
    {
        switch (init) {
            case InitClosest : FStartPath(Lesser,insprob);  break;
            case InitFurthest: FStartPath(Greater,insprob); break;
            default          : RndStartPath(insprob);       break;
        }
                    
        switch (method) {
            case InsCheap   : return ControlCheap(insprob,graphics);    break;
            case InsDear    : return ControlDear(insprob,graphics);     break;
            case InsNearest : return ControlNearest(insprob,graphics);  break;
            case InsFurthest: return ControlFurthest(insprob,graphics); break;
            default/*Rand*/ : return ControlRand(insprob,graphics);     break;
        }
    }
}
