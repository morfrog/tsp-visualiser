#include <stdlib.h>
#include <stdio.h>
#include <graph.h>
/* #include <values.h> */
#include <time.h>
#include <conio.h>
#include "screen.h"
#include "file.h"
#include "path.h"

#define DEBUG 1
#define PRINTON 1
#define TEST_RUNS 500

int random(int num)
{
    return rand() % num;
}

char mygetch()
{
        if (DEBUG) return getch();
        return 0;                       
}

int PathLength(PATH *p,DISTANCES *d)
{
        int count,ret=0;
    NODE look=p->head;

    for (count=0;count<p->size;count++)
    {
        ret+=d->array[look->citynum][look->next->citynum];
        look=look->next;
    }
    return ret;
}

void CreateRandomOrder(int order[],int size)
{

    int count;
    int tmp,source,target; /* for swapping values */
    
    for (count=0;count<size;count++)
        order[count]=count;
    for (count=0;count<(size*3);count++) /* perform size*3 swaps is good */
    {
        target=random(size);
        source=random(size);
        if (source!=target)  /* swap them */
        {
            tmp=order[target];
            order[target]=order[source];
            order[source]=tmp;
        }
        else ++count;
    }
    /*for (count=0;count<size;count++)
        printf("%d ",order[count]);*/
        
}

PATH *InsertMethod(DISTANCES *d)
/* Generates a path from the distances matrix using the basic insert method -
 * RULES :- 1. pick two points as initial path
 *          2. add points to path such that resulting path size increase is 
 *             minimised
 */
{
    PATH *p;
    NODE Nc,Nd;
    int order[20];
    int order_pos; 
    int count,insert_city,this_dist;
    int bestA,bestB,best_dist; /* > biggest possible */
    int total;

    CreateRandomOrder(&order,d->size);
    p=NewPath(order[0],order[1],order[2]);
    total=d->array[order[0]][order[1]]+
          d->array[order[1]][order[2]]+
          d->array[order[2]][order[0]];
    PathStart(p);
    if (PRINTON) printf("Start Path IS ");
    ShowPath(p);

    for (order_pos=3;order_pos<d->size;order_pos++) /* 0,1 and 2 are start path */
    {
        insert_city=order[order_pos];
        if (PRINTON) printf("Inserting City %d.\n",insert_city);
        best_dist=1000;
        PathStart(p);

        for (count=0;count<p->size;count++)
        {
            Nc=p->current;
            Nd=p->current->next;
            this_dist=
                d->array[insert_city][Nc->citynum] + /* add new */
                d->array[insert_city][Nd->citynum]      - /* subtract old */
                d->array[Nc->citynum][Nd->citynum];
        
            if (PRINTON) printf("Distance %d=%d+%d-%d.\n",this_dist,
                d->array[insert_city][Nc->citynum],
                d->array[insert_city][Nd->citynum],
                d->array[Nc->citynum][Nd->citynum]);

            if (this_dist<best_dist)
            {
                best_dist=this_dist;
                bestA=Nc->citynum;
                bestB=Nd->citynum;
            }

            PathNext(p);
        
        }
        if (PRINTON)
            printf("Best was %d -> %d, Distance %d, Proceeding with next Insert.\n",
                            bestA,bestB,       best_dist);
        total+=best_dist;
        if (!(InsertPath(p,insert_city,bestA,bestB)))
        {
            printf("Improper Insert...Exiting.\n");
            exit(EXIT_FAILURE);
        }
        ShowPath(p);
        mygetch();
    }
    if (PRINTON) printf("Total Distance Is %d\n",total);                        
    return p;
}


void DrawCities(LOCATES loc)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<=(loc->num)-1;count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
        DrawCity(num,loc->at[count].x,loc->at[count].y,12);
    }
}

void DrawPath(PATH *p,LOCATES loc)
{
    NODE c=p->head;
    int count;

    for (count=0;count<(p->size);count++)
    {
        DrawLine(loc->at[c->citynum].x,      loc->at[c->citynum].y,
                 loc->at[c->next->citynum].x,loc->at[c->next->citynum].y);
        c=c->next;      
    }
}


void main(int argc, char *argv[])
{
    DISTANCES *d;
    PATH *p;
    /* char str[40]; */
    int length,count,best=10000,worst=0;
    long avg_tot=0,avg_num=0;

    
    if (argc<2)
    {
        printf("Visual Tool For The Travelling Salesman Problem.\n");
        printf("Usage: %s filename.",argv[0]);
    }
    else
    {
        printf("Reading Map...\n");
        d=NewMap(argv[1]);
        printf("Map Read...\n");
        PrintMap(d);
        mygetch();

        for (count=0;count<TEST_RUNS;count++)
        {
            printf("-----------START-------------\n");
            p=InsertMethod(d);
            length=PathLength(p,d);
            
            if (length<best) best=length;
            if (length>worst) worst=length;
            avg_tot+=length;
            ++avg_num;
            ShowPath(p);
            FreePath(p);
            
        }
        printf("-------------------End-----------------\n");
        printf("   Best Path Length Off %3d Runs was %3d\n",TEST_RUNS,best);
        printf("Average Path Length Off %3d Runs was %3d\n",TEST_RUNS,avg_tot / avg_num);
        printf("  Worst Path Length Off %3d Runs was %3d\n",TEST_RUNS,worst);
        getch();

        InitScreen();
        _setcolor(normal_col);
        _rectangle(_GBORDER,0,0,639,479);
   
        DrawPath(p,d->l);
        DrawCities(d->l);
   
        mygetch();
        ShutDown();
        FreeMap(d);
        FreePath(p);
        
    }
}


/*
void main()
{
    int order[20];
    int count;

    printf("--------------\n");
        CreateRandomOrder(order,10);
        for (count=0;count<10;count++) printf("%d ",order[count]);
    printf("\n");
        CreateRandomOrder(order,10);
        for (count=0;count<10;count++) printf("%d ",order[count]);
    printf("\n");


}
*/
