#ifndef PATH_H

// * Is now a double linked list * //

#define Next 0;
#define Prev 1;

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
        struct node *prev;
};

typedef struct node NODE;

struct header
{
    NODE *head;
    NODE *current;  // * the current * //
    NODE *save;    // * the save node -> for temporary deletion * //
    int washead;   // * true if the save node was the head * //
    int size;
};

typedef struct header PATH;

PATH _FAR *NewPath(int points, ...);
void _FAR FreePath(PATH *p);
void _FAR InsertPathEnd(PATH *path ,int city);
int _FAR InsertPath(PATH *p,int city,int city1,int city2);
int _FAR DeletePath(PATH *p,int city);
void _FAR ShowPath(PATH *p);

int TmpPathDel(PATH *p,int citynum,int *A,int *B);
int TmpPathIns(PATH *p);
int TmpCity(PATH *p);

void _FAR PathStart(PATH *p);
void _FAR PathEnd(PATH *p);
void _FAR PathCurrent(PATH *p,int *a);
void _FAR PathPair(PATH *p,int *a,int *b);
void _FAR PathNext(PATH *p);

int ReverseSubPath(PATH *p,int keepl,int subl,int subr,int keepr);

int _FAR *PathToArray(PATH *p,int circlular);
PATH _FAR *ArrayToPath(int *array,int size,int circular);
int _FAR EqualPaths(PATH *a,PATH *b);

#endif
