#ifndef PATH_H

#if defined ( __386__ )
    #define _FAR
    #define _HUGE
#else
    #define _FAR    __far
    #define _HUGE   __huge
#endif

struct path_node 
{
        struct path_node *next;
        int citynum;
};

typedef struct path_node NODE;

struct path_header
{
    NODE *head;
    NODE *current; /* the current */
    int size;
};

typedef struct path_header PATH;

PATH _FAR *NewPath(int city1,int city2,int city3);
void _FAR FreePath(PATH *p);
int _FAR InsertPath(PATH *p,int city,int city1,int city2);
void _FAR ShowPath(PATH *p);
void _FAR PathStart(PATH *p);
void _FAR PathNext(PATH *p);
int _FAR *PathToArray(PATH *p);
int _FAR EqualPaths(PATH *a,PATH *b);

#endif
