#ifndef INSERT_H

#define InitNearN   -1
#define InitRand     0
#define InitClosest  1
#define InitFurthest 2

#define InsNearN   -1
#define InsRand     0
#define InsCheap    1
#define InsDear     2
#define InsNearest  3
#define InsFurthest 4

#define NoGraphics 0
#define Graphics   1

struct iprob
{
    PROBLEM *prob;
    int endo;
    int *outpath;
};

typedef struct iprob INSPROB;

void   InsProbReset(INSPROB *insprob,int free);
INSPROB *NewInsProb(DISTANCES *dist);
void   CityDel(INSPROB *insprob,int citynum);
void   NNeighbourStartPath(INSPROB *insprob);
void   RndStartPath(INSPROB *insprob);
int    FNeighbour(int (* func)(double,double),INSPROB *insprob,int citynum);
void   FStartPath(int (* func)(double,double),INSPROB *insprob);
int    FNextCost(int (* func)(double,double),INSPROB *insprob,int *A,int *B);
int    FNextDist(int (* func)(double,double),int (* func2)(double,double),INSPROB *insprob,int *fcity);
int    RndNext(INSPROB *insprob);
char   *InitStr(int init);
char   *MethodStr(int method);
double ControlRand(INSPROB *insprob,int graphics);
double ControlNearN(INSPROB *insprob,int graphics);
double ControlFurthest(INSPROB *insprob,int graphics);
double ControlNearest(INSPROB *insprob,int graphics);
double ControlDear(INSPROB *insprob,int graphics);
double ControlCheap(INSPROB *insprob,int graphics);
double ControlAllIns(INSPROB *insprob,int init,int method,int graphics);
#endif
