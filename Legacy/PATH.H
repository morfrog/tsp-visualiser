#ifndef PATH_H

#if defined ( __386__ )
    #define _FAR
    #define _HUGE
#else
    #define _FAR    __far
    #define _HUGE   __huge
#endif

struct node 
{
        int citynum;
        struct node *next;
};

typedef struct node NODE;

struct header
{
    NODE *head;
    NODE *tail;
    NODE *current; /* the current */
    int size;
};

typedef struct header PATH;

PATH _FAR *NewPath(int points, ...);
void _FAR FreePath(PATH *p);
void _FAR InsertPathEnd(PATH *path ,int city);
int _FAR InsertPath(PATH *p,int city,int city1,int city2);
void _FAR ShowPath(PATH *p);
void _FAR PathStart(PATH *p);
void _FAR PathEnd(PATH *p);
void _FAR PathCurrent(PATH *p,int *a);
void _FAR PathPair(PATH *p,int *a,int *b);
void _FAR PathNext(PATH *p);
int _FAR *PathToArray(PATH *p);
int _FAR EqualPaths(PATH *a,PATH *b);

#endif
