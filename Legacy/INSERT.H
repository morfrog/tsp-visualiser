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

double InsertCost(PROBLEM *prob,int I, int A, int B);
void   BestInsert(PROBLEM *prob,int citynum,int *a,int *b);
void   NNeighbourStartPath(PROBLEM *prob);
void   RndStartPath(PROBLEM *prob);
int    FNeighbour(int (* func)(double,double),PROBLEM *prob,int citynum);
void   FStartPath(int (* func)(double,double),PROBLEM *prob);
int    FNextCost(int (* func)(double,double),PROBLEM *prob,int *A,int *B);
int    FNextDist(int (* func)(double,double),int (* func2)(double,double),PROBLEM *prob,int *fcity);
int    RndNext(PROBLEM *prob);
void   PrintInit(int init);
void   PrintMethod(int method);
double ControlRand(PROBLEM *prob,int graphics);
double ControlNearN(PROBLEM *prob,int graphics);
double ControlFurthest(PROBLEM *prob,int graphics);
double ControlNearest(PROBLEM *prob,int graphics);
double ControlDear(PROBLEM *prob,int graphics);
double ControlCheap(PROBLEM *prob,int graphics);

#endif
