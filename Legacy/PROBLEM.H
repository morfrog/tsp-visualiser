#ifndef PATH_H

#include "file.h" /* DISTANCES */
#include "path.h" /* PATH */

struct prob {
    DISTANCES *dist;
    int endi;
    int *inpath;
    PATH *path;
    int endo;
    int *outpath;
};

typedef struct prob PROBLEM;

PROBLEM *NewProblem(DISTANCES *d);
double PathLength(PATH *p,DISTANCES *d);
PATH *InsertMethod(DISTANCES *d);

#endif
