#include <stdlib.h>
#include <stdio.h>
#include "sys.h"
#include "path.h"

PATH _FAR *NewPath(int city1,int city2,int city3)
{
    NODE *CityOne=salloc(1,sizeof(NODE));
    NODE *CityTwo=salloc(1,sizeof(NODE));
    NODE *CityThree=salloc(1,sizeof(NODE));
    PATH *t=salloc(1,sizeof(PATH));
    t->head=CityOne;
    t->current=CityOne;
    t->size=3;

    CityOne->citynum=city1;
    CityTwo->citynum=city2;
    CityThree->citynum=city3;

    CityOne->next=CityTwo;
    CityTwo->next=CityThree;
    CityThree->next=CityOne;

    return t;
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
    if (sfree(p)) p=NULL;
    else printf("Failed To Free Path Structure.\n");



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
                new->next=c->next;
                c->next=new;
                p->size=(p->size)+1;
                return 1;
        }
        c=c->next;
    }
        return 0;
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
