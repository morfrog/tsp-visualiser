#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file.h"
#include "path.h"
#include "sys.h"
#include "problem.h"


PROBLEM *NewProblem(DISTANCES *dist)
{
    int x;
    PROBLEM *ret=salloc(1,sizeof(PROBLEM));

    ret->dist=dist;

    ret->endi=0;
    ret->inpath=salloc((dist->size),sizeof(int));

    ret->endo=(dist->size);
    ret->outpath=salloc((dist->size),sizeof(int));

    for (x=0;x<(dist->size);x++)
        ret->outpath[x]=x;

    ret->path=NULL;
    return ret;
}

double PathLength(PATH *p,DISTANCES *d)
{
    int count;
    double ret=0.0;
    NODE *look=p->head;

    if (p==NULL) return ret;

    for (count=0;count<p->size;count++)
    {
        ret+=d->array[look->citynum][look->next->citynum];
        look=look->next;
    }
    return ret;
}

void RndStartPath(PROBLEM *prob)
{
    


}

