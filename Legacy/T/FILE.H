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

struct city
{
        char name[20];
};      

struct point
{
        int x;
    int y;
    int citynum;
};

typedef struct point POINT;

struct locates
{
        int num;
        POINT *at; /* array of locations */
};

typedef struct locates *LOCATES;

struct matrix
{
        int size; /* <=big */
        int array[big][big];
    LOCATES l;
};

typedef struct matrix DISTANCES;

void _FAR ReadToken(FILE *f,char string[],int max);
void _FAR SkipTill(FILE *f,char this);
void _FAR SkipLine(FILE *f);
int _FAR ReadInt(FILE *f);
int _FAR Dist(POINT a,POINT b);
DISTANCES *NewMap(char *filename);
void _FAR FreeMap(DISTANCES *d);
void _FAR PrintMap(DISTANCES *d);

#endif
