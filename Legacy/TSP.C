#include <stdlib.h>
#include <stdio.h>
#include <graph.h>
/* #include <values.h> */
#include <time.h>
#include <conio.h>
#include <i86.h>
#include "sys.h"
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

double PathLength(PATH *p,DISTANCES *d)
{
    int count;
    double ret=0.0;
    NODE *look=p->head;

    for (count=0;count<p->size;count++)
    {
        ret+=d->array[look->citynum][look->next->citynum];
        look=look->next;
    }
    return ret;
}

int *CreateRandomOrder(int rsize)
{
    int count,tmp,source,target; /* for swapping values */
    int *ret=NULL;

    ret=salloc(rsize,sizeof(int));
   
    printf("Starting Randomize\n");
    for (count=0;count<rsize;count++)
        ret[count]=count;
    for (count=0;count<rsize;count++)
        printf("%2d ",ret[count]);
    printf("\n");
    for (count=0;count<(rsize*3);count++) /* perform size*3 swaps is good */
    {
        target=random(rsize);
        source=random(rsize);
        if (source!=target)  /* swap them */
        {
            tmp=ret[target];
            ret[target]=ret[source];
            ret[source]=tmp;
        }
        else ++count;
    }
    for (count=0;count<rsize;count++)
        printf("%2d ",ret[count]);
    printf("\n");
    printf("First Three are %d %d %d",ret[0],ret[1],ret[2]);
    return ret;        
}

PATH *InsertMethod(DISTANCES *d)
/* Generates a path from the distances matrix using the basic insert method -
 * RULES :- 1. pick two points as initial path
 *          2. add points to path such that resulting path size increase is 
 *             minimised
 */
{
    PATH *p=NULL;
    NODE *Nc,*Nd;
    int *order=NULL;
    int order_pos,count;
    double this_dist,
           best_dist, /* > biggest possible */
           total;
    int bestA,bestB,insert_city; /* cities */

 

    printf("Creating Random Order \n");
    order=CreateRandomOrder((d->size));

    printf("Creating New Path %d -> %d -> %d\n",order[0],order[1],order[2]);

    p=NewPath(order[0],order[1],order[2]);

    total=d->array[order[0]][order[1]]+
          d->array[order[1]][order[2]]+
          d->array[order[2]][order[0]];
    PathStart(p);
    if (PRINTON) printf("Start Path IS ");
    ShowPath(p);


    for (order_pos=3;order_pos<(d->size);order_pos++) /* 0,1 and 2 are start path */
    {
        insert_city=order[order_pos];
        if (PRINTON) printf("Inserting City %d.\n",insert_city);
        best_dist=1000.0;
        PathStart(p);

        for (count=0;count<(p->size);count++)
        {
            Nc=p->current;
            Nd=p->current->next;
            this_dist=
                d->array[insert_city][Nc->citynum] + /* add new */
                d->array[insert_city][Nd->citynum]      - /* subtract old */
                d->array[Nc->citynum][Nd->citynum];
        
            if (PRINTON) printf("Distance %.4f=%.4f+%.4f-%.4f.\n",this_dist,
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
            printf("Best was %d -> %d, Distance %.4f, Proceeding with next Insert.\n",
                            bestA,bestB,       best_dist);
        total+=best_dist;
        if (!(InsertPath(p,insert_city,bestA,bestB)))
        {
            printf("Improper Insert...Exiting.\n");
            exit(EXIT_FAILURE);
        }
        ShowPath(p);
    }
    if (PRINTON) printf("Total Distance Is %.4f\n",total);                        
    return p;
}

void DrawCities(LOCATES *loc)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<=(loc->num)-1;count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
                DrawCity(num,loc->at[count].x,loc->at[count].y,0.03);
    }
}

void DrawPath(PATH *p,LOCATES *loc)
{
    NODE *c=p->head;
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
    DISTANCES *d=NULL;
    PATH *p=NULL;
    SWIN *main=NewWin(1,1,350,350);
    /* char str[40]; */
    double length,best=10000.0,worst=0.0;
    int count;
    double avg_tot=0.0,avg_num=0.0;
    int *tmp;

/*
    printf("POINT     : %d  * %d\nDISTANCES : %d  * %d\n",
    sizeof(POINT),sizeof(POINT *),sizeof(DISTANCES),sizeof(DISTANCES *));
    mygetch();
*/
    if (argc<2)
    {
        printf("Visual Tool For The Travelling Salesman Problem.\n");
        printf("Usage: %s filename [time].\n%s\n%s\n%s\n",argv[0],
               "Where        time = 0 is calculate to end",
               "             time = n is wait n seconds between each insert",
               "          No time = wait for keypress between each insert");
    }
    else
    {
        printf("Reading Map From File '%s'...\n",argv[1]);
        d=NewMap(argv[1]);
        printf("Map Read...\n");
        PrintMap(d);
        mygetch();

        /*
        for (count=0;count<TEST_RUNS;count++)
        {
            printf("-----------START-------------\n");
            p=InsertMethod(d);
            printf("Insert Method Complete.");
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
        mygetch();
        */
       
        printf("Insert Method Starting...\n");
        p=InsertMethod(d);
        printf("Insert Method Complete - Calculating Path Length.\n");

        length=PathLength(p,d);
        printf("Length was %.3f\n",length);
        mygetch();


        InitScreen(16);
        /*
        _setcolor(normal_col);
        _rectangle(_GBORDER,0,0,639,349);
        */
        SetWin(main);
        BorderWin();

        DrawPath(p,d->l);

        DrawCities(d->l);

        delay(2000);
        
        ShutDown();
/*      FreeWin(main);*/
     
/*      FreeMap(d); */
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
