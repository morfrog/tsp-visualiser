#ifndef PROBLEM_H

#include <time.h>

#define REQUEST -2
#define WAITKEY -1
#define WAIT_EXIT 0
#define WAIT_OK 1

int waitresult=WAIT_OK;

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

int     TotalDelay(int tdelay);
int     WaitTill(clock_t it);
void    Init2TimeSegs(clock_t *a,clock_t *b);
void    Init3TimeSegs(clock_t *a,clock_t *b,clock_t *c);
void    DrawCitiesLine(LOCATES *loc,int A,int B,int col);
void    DrawCityN(LOCATES *loc,int n,double size,int color);
void    DrawCities(LOCATES *loc,MASK *m);
void    DrawMaskPath(PATH *p,LOCATES *loc,MASK *m);
void    DrawPath(PATH *p,LOCATES *loc,int returnp);
char    *DelayStr(int delay,double optimal);
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
void _FAR SetMasksFrom(MASK *m,int *list,int listsize,int col, double size);
int  _FAR SetMasks(MASK *m,int num, ...); // int pos,int color,double size
void    DrawMaskPath(PATH *p,LOCATES *loc,MASK *m);
LOCATES *PathAsLocates(PATH *path,LOCATES *loc);
PATH    *LocatesAsPath(LOCATES *loc);
int ControlLCLOverlay(PROBLEM *one,PROBLEM *two,int width,int height,int starty,int col1,int col2);
int ControlVarOverlay(PROBLEM *prob,int startx,int starty,int endx,int endy,int col,
                      int l,int t,int r,int b,int (* func)(int,int),int stopwidth,
                      int stopheight,int stoptimes);
void _FAR ShowText(char string[],int start,int line,int color);
void WriteScreen2File(void);
#endif
