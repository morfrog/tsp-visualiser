#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "sys.h"
#include "path.h"

NODE *FindPathCity(PATH *p,int citynum,int *pos)
// * returns NULL if city not in path else returns the pointer to the node * //
{
    NODE *ret=p->head;
    int x,found=0;

    ret=p->head;
    for (x=0;x<p->size;x++) {
        if (ret->citynum==citynum) {found=1;*pos=x;break;}
        ret=ret->next;
    }
    if (!found) return NULL; // * city not in path * //
    return ret;
}

int FindNodes(PATH *p,int city1,NODE **c1,int city2,NODE **c2)
{
    NODE *look=p->head;
    int x;
    *c1=NULL;
    *c2=NULL;

    for (x=0;x<(p->size);x++)
    {
        if (look->citynum==city1) *c1=look;
        if (look->citynum==city2) *c2=look;
        if (((*c1)!=NULL) && ((*c2)!=NULL)) return 1;
        look=look->next;
    }
    return 0;
}

int NodalDist(NODE *a,NODE *b,int max)
{
    NODE *look=a;
    int ret=0,x;
    for (x=0;x<max;x++)
    {
        if (look==b) return ret;
        else {++ret;look=look->next;}
    }
    return -1;
}


void Reversal(PATH *p,NODE *left,NODE *right)
{
    NODE *look1=left;
    NODE *look2=right;
    int tmp;
    int x;
    int swaps=(NodalDist(left,right,p->size)+1)/2;
    
    for (x=0;x<swaps;x++)
    {
        tmp=look1->citynum;
        look1->citynum=look2->citynum;
        look2->citynum=tmp;

        look1=look1->next;
        look2=look2->prev;
    }
}

int ReverseSubPath(PATH *p,int keepl,int subl,int subr,int keepr)
{
    NODE *a,*b;

    FindNodes(p,subl,&a,subr,&b);
    if ((a->prev->citynum==keepl) && (b->next->citynum==keepr))
        Reversal(p,a,b);
    else if ((a->next->citynum==keepl) && (b->prev->citynum==keepr))
        Reversal(p,b,a);
    else return 0;
    return 1;
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
    int t;
    NODE *tmp=FindPathCity(p,city,&t);
    if (tmp==NULL) return 0;
    tmp->prev->next=tmp->next;
    tmp->next->prev=tmp->prev;
    if (p->head==tmp) p->head=tmp->next;
    sfree(tmp);
    p->size-=1;
    return 1;
}

PATH *RemovePathSection(PATH *p,int A,int B,int C, int D)
{
    PATH *ret=NULL;
    NODE *look=p->head;
    NODE *nodeA=NULL,*nodeB=NULL,*nodeC=NULL,*nodeD=NULL;
    int pA,pB,pC,pD;
    nodeA=FindPathCity(p,A,&pA);
    nodeB=FindPathCity(p,B,&pB);
    nodeC=FindPathCity(p,C,&pC);
    nodeD=FindPathCity(p,D,&pD);
    if ((nodeA->next==nodeB) && (nodeC->next==nodeD))
    {
        ret=salloc(1,sizeof(PATH));
        if (pB<pC)
        {
            ret->size=pC-pB;
            ret->head=nodeB;
        }
        else
        {
            ret->size=pB-pC;
        }
        return ret;
    }
    else return NULL; // * integrity absent - abort * //
}

void InsertPathSection(PATH *p,int citystart,int cityend)
{


}

void ReversePath(PATH *p)
{


}

int TmpPathDel(PATH *p,int citynum,int *A,int *B)
{
    int f;
    if (p->save!=NULL) return 0; // * already has a node saved * //
    
    p->save=FindPathCity(p,citynum,&f);
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

int _FAR *PathToArray(PATH *p,int circular)
{
    int *array=salloc(p->size+(circular?1:0),sizeof(int));
    NODE *look=p->head;
    int count;

    for (count=0;count<p->size;count++)
    {
        array[count]=(look->citynum);
        look=look->next;
    }
    if (circular) array[p->size]=array[0];
    return array;
}

PATH _FAR *ArrayToPath(int *array,int size,int circular)
{
    int max=size-(circular?1:0),x;
    PATH *ret=NewPath(3,array[0],array[1],array[2]);
    for (x=3;x<max;x++)
        InsertPathEnd(ret,array[x]);
    return ret;
}


int _FAR EqualPaths(PATH *a,PATH *b)
/* returns 1 if the physical path of a is equal to the physical path of b
 * returns 0 otherwise.
 */
{
    int count,equal_found=0;
    int *aarray=PathToArray(a,1);
    int *barray=PathToArray(b,1);

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
    NODE *a,*b;
    PATH *path=NewPath(10,0,1,2,3,4,5,6,7,8,9);
   
    ShowPath(path);
    FindNodes(path,0,&a,9,&b);
    printf("%d %d\ndist=%d\n",a->citynum,b->citynum,NodalDist(a,b,10));
    ReverseSubPath(path,8,9,2,3);
    printf("1 0 9 3 4 5 6 7 8 2\n");ShowPath(path);
    mygetch();
}
*/
