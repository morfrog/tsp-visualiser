#ifndef PROBLEM_H

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
    int endo;
    int *outpath;
    MASK *vmask;
};

typedef struct prob PROBLEM;

PROBLEM *NewProblem(DISTANCES *d);
double PathLength(PATH *p,DISTANCES *d);
double InsertCost(PROBLEM *prob,int I, int A, int B);
void BestInsert(PROBLEM *prob,int citynum,int *a,int *b);
void ProblemReset(PROBLEM *prob,int free);
void PrintList(int *list,int listsize);
void CityDel(PROBLEM *prob,int citynum);
void NNeighbourStartPath(PROBLEM *prob);
void RndStartPath(PROBLEM *prob);
int FNeighbour(int (* func)(double,double),PROBLEM *prob,int citynum);
int Lesser(double this,double best);
int Greater(double this,double best);
void FStartPath(int (* func)(double,double),PROBLEM *prob);
int FNextCost(int (* func)(double,double),PROBLEM *prob,int *A,int *B);
int FNextDist(int (* func)(double,double),int (* func2)(double,double),PROBLEM *prob,int *fcity);
int RndNext(PROBLEM *prob);
MASK _FAR *NewViewMask(int size,int color,double csize);
void _FAR SetMask(MASK *m,int color,double size);
void _FAR SetMaskCol(MASK *m,int color);
void _FAR SetMaskSize(MASK *m,double size);
int _FAR SetMaskAt(MASK *m,int color,double size,int pos);

#endif
