#ifndef FILE_H
#define big 10
#define STR_LEN 10

#if defined ( __386__ )
    #define _FAR
    #define _HUGE
#else
    #define _FAR    __far
    #define _HUGE   __huge
#endif

struct point
{
    double x,y;
    int citynum;
};

typedef struct point POINT;

struct locates
{
        int num;
        POINT *at; /* array of locations */
};

typedef struct locates LOCATES;

struct matrix
{
        int size;
        LOCATES *l;
        double **array;
};

typedef struct matrix DISTANCES;

void ReadToken(FILE *f,char string[],int max);
void SkipTill(FILE *f,char this);
void SkipLine(FILE *f);
double ReadDouble(FILE *f);
double Dist(POINT a,POINT b);
DISTANCES *NewMap(char *filename);
void FreeMap(DISTANCES *d);
void PrintMap(DISTANCES *d);
void ShowLocates(LOCATES *loc);
LOCATES *CopyLocates(LOCATES *loc);
#endif
