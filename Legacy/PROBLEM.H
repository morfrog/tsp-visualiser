#ifndef PROBLEM_H

#include <time.h>

struct vnode
{
    int color;
    double size;
};

typedef struct vnode VNODE;

struct vmask
{
    int size;
    VNODE *array; /* array of VNODE's */
};

typedef struct vmask MASK;

struct prob {
    DISTANCES *dist;
    PATH *path;
    MASK *vmask;
};

typedef struct prob PROBLEM;

void    WaitTill(clock_t it);
void    DrawCities(LOCATES *loc,MASK *m);
void    DrawPath(PATH *p,LOCATES *loc,int returnp);
PROBLEM *NewProblem(DISTANCES *d);
double  PathLength(PATH *p,DISTANCES *d);
void    ProblemReset(PROBLEM *prob,int free);
double  InsertCost(PROBLEM *prob,int I, int A, int B);
void    BestInsert(PROBLEM *prob,int citynum,int *a,int *b);
void    PrintList(int *list,int listsize);
void    HiliteCityGroup(PROBLEM *prob,int main,int maincol,int city1, int city2,int col);
void    HiliteCityPair(PROBLEM *prob,int city1,int city2,int col);
void    HiliteCity(PROBLEM *prob,int citynum,int returnp);
void    DrawNorm(PROBLEM *prob,int returnp);
MASK _FAR *NewViewMask(int size,int color,double csize);
void _FAR SetMask(MASK *m,int color,double size);
void _FAR SetMaskCol(MASK *m,int color);
void _FAR SetMaskSize(MASK *m,double size);
int  _FAR SetMaskAt(MASK *m,int color,double size,int pos);
int  _FAR SetMasks(MASK *m,int num, ...); // int pos,int color,double size
int     TotalDelay(int tdelay);
LOCATES *PathAsLocates(PATH *path,LOCATES *loc);
PATH    *LocatesAsPath(LOCATES *loc);
#endif
