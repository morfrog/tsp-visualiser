#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sys.h"
#include "path.h"

NODE *FindPathCity(PATH *p,int citynum)
// * returns NULL if city not in path else returns the pointer to the node * //
{
    NODE *ret=p->head;
    int x,found=0;

    ret=p->head;
    for (x=0;x<p->size;x++) {
        if (ret->citynum==citynum) {found=1;break;}
        ret=ret->next;
    }
    if (!found) return NULL; // * city not in path * //
    return ret;
}

PATH _FAR *NewPath(int points, ...)
{
    int x,a;
    NODE *ins=NULL;
    PATH *path=salloc(1,sizeof(PATH));
    va_list ap;

    path->head=NULL;
    path->current=NULL;
    path->save=NULL;
    path->washead=0;
    path->size=0;
    va_start(ap,points);

    for (x=0;x<points;x++)
    {
        a=va_arg(ap, int);
        ins=salloc(1,sizeof(NODE));
        ins->citynum=a;
        if (path->size==0) {
            path->head=ins;
        }
        else
        {
            path->head->prev->next=ins;
            ins->prev=path->head->prev;
        }
        ins->next=path->head;
        path->head->prev=ins;

        path->size+=1;
    }
    va_end(ap);
    return path;
}

void _FAR FreePath(PATH *p)
{
    NODE *c=p->head->next;
    NODE *del=p->head;
    int count;

    for (count=1;count<(p->size);count++)
    {
        if (!(sfree(del))) printf("Failed To Free Path Node.\n");
        del=c;  
        c=c->next;
    }
    if (!(sfree(p))) printf("Failed To Free Path Structure.\n");
}

void _FAR InsertPathEnd(PATH *path,int city)
{
    NODE *new=salloc(1,sizeof(NODE));

    new->citynum=city;
    path->head->prev->next=new;
    new->prev=path->head->prev;
    new->next=path->head;
    path->head->prev=new;
    path->size+=1;
}

int _FAR InsertPath(PATH *p,int city,int city1,int city2)
/* inserts city between city1 and city2, returns 1 if works, 0 if not */
{
    NODE *c=p->head;
    NODE *new=NULL;
    int count;

    for (count=0;count<p->size;count++)
    {
        if (((c->citynum==city1) && (c->next->citynum==city2)) ||
            ((c->citynum==city2) && (c->next->citynum==city1)))
        {
                new=salloc(1,sizeof(NODE));
                new->citynum=city;

                new->prev=c;
                new->next=c->next;
                c->next->prev=new;
                c->next=new;
                p->size+=1;
                return 1;
        }
        c=c->next;
    }
        return 0;
}
int _FAR DeletePath(PATH *p,int city)
{
    NODE *tmp=FindPathCity(p,city);
    if (tmp==NULL) return 0;
    tmp->prev->next=tmp->next;
    tmp->next->prev=tmp->prev;
    if (p->head==tmp) p->head=tmp->next;
    sfree(tmp);
    return 1;
}

int TmpPathDel(PATH *p,int citynum,int *A,int *B)
{

    if (p->save!=NULL) return 0; // * already has a node saved * //
    
    p->save=FindPathCity(p,citynum);
    if (p->save==NULL) return 0; // * city not in path * //

    *A=p->save->prev->citynum;
    *B=p->save->next->citynum;
    
    p->save->prev->next=p->save->next;
    p->save->next->prev=p->save->prev;

    if (p->save==p->head) {
        p->washead=1;
        p->head=p->save->next;
    }
    else p->washead=0;

    if (p->save==p->current) p->current=p->save->next;
   
    p->size-=1;
    return 1;
}

int TmpPathIns(PATH *p)
// * Reinserts Temporarily Removed city back in its old place if * //
// * possible * //
{
    if (p->save->prev->next!=p->save->next) return 0;
    if (p->washead) p->head=p->save;
    p->save->prev->next=p->save;
    p->save->next->prev=p->save;
    p->save=NULL;  // * now available for tmp deletion * //
    p->size+=1;


    return 1;
}

int TmpCity(PATH *p)
{
    if (p->save==NULL) return -1;
    return p->save->citynum;
}

void _FAR ShowPath(PATH *p)
{
    NODE *c=p->head;
    int count;

    for (count=1;count<(p->size);count++)
    {
                printf("%d -> ",c->citynum);
                c=c->next;      
    }
    printf("%d\n",c->citynum);

}

void _FAR PathStart(PATH *p)
{
    p->current=p->head;
}

void _FAR PathEnd(PATH *p)
{
    p->current=p->head->prev;
}

void _FAR PathCurrent(PATH *p,int *a)
{
    *a=p->current->citynum;
}

void _FAR PathPair(PATH *p,int *a,int *b)
{
    *a=p->current->citynum;
    *b=p->current->next->citynum;
}

void _FAR PathNext(PATH *p)
/* advances the currents */
{
    p->current=p->current->next;
}

int _FAR *PathToArray(PATH *p)
{
    int *array=salloc(p->size,sizeof(int));
    NODE *look=p->head;
    int count;

    for (count=0;count<p->size;count++)
    {
        array[count]=(look->citynum);
        look=look->next;
    }
    return array;
}


int _FAR EqualPaths(PATH *a,PATH *b)
/* returns 1 if the physical path of a is equal to the physical path of b
 * returns 0 otherwise.
 */
{
    int count,equal_found=0;
    int *aarray=PathToArray(a);
    int *barray=PathToArray(b);

    if ((a->size)!=(b->size))
        return 0; /* different size therefore not equal */
    else
        printf("Same Size\n");


    for (count=0;count<(a->size);count++)
    {
        if ((aarray[0])==(barray[count]))
        {
            equal_found=1;
            printf("Common %d Found",barray[count]);
                break;
        }
        else
        {
                printf("NEXT..%d..",barray[count]);
        }
    }
    if (!(equal_found)) 
                return 0; /* a city which appears in one is not in the other */

    for (count=0;count<(a->size);count++)
        {
        


    }
    return 1;
}



/*
void main()
{
    PATH *path=NewPath(2,5,0);
    PATH *new=NewPath(2,7,8);
    int **a=NULL;

    printf("-----------------\n");
    ShowPath(path);
    ShowPath(new);
}
*/
