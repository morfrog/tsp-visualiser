#include <stdlib.h>
#include <stdio.h>
#include <graph.h>
#include <time.h>
#include <conio.h>
#include <i86.h>
#include <dos.h>
#include "sys.h"
#include "screen.h"
#include "file.h"
#include "path.h"
#include "problem.h"
#include "consts.h"
 
#define DEBUG 1
#define PRINTON 0
#define TEST_RUNS 500

int wait_time;

void WaitTill(clock_t it)
{
    if (wait_time<0) getch();
    else
    {
        while (clock()<it)
        {
            if (kbhit()) {getch();break;}
        }
    }
}

void DrawCities(LOCATES *loc)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<=(loc->num)-1;count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
                DrawCity(num,loc->at[count].x,loc->at[count].y,city_size,city_col);
    }
}

void DrawPath(PATH *p,LOCATES *loc)
{
    NODE *c=p->head;
    int count;

    for (count=0;count<(p->size);count++)
    {
        DrawLine(loc->at[c->citynum].x,      loc->at[c->citynum].y,
                 loc->at[c->next->citynum].x,loc->at[c->next->citynum].y,
                 line_col);
        c=c->next;      
    }
}


void main(int argc, char *argv[])
{
    DISTANCES *d=NULL;
    PATH *p=NULL;
    SWIN *main=NewWin(1,1,350,460);
    /* char str[40]; */
    double length,best=10000.0,worst=0.0;
    int count;
    double avg_tot=0.0,avg_num=0.0;
    int *tmp;
    int col;
    

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
        if (argc==3)
        {
            wait_time=atoi(argv[2]);
            if (wait_time)
            {
                printf("Running With Time Argument %d.\n",wait_time);
                wait_time=wait_time*CLOCKS_PER_SEC;
                printf("Which is %d ticks.\n",wait_time);
            }
            else printf("Running Through To End.\n");
        }
        else
        {
            wait_time=-1;
            printf("Running With Wait-For-Keypress Option.\n");
        }

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
       
        InitScreen(16);
        SwapPage();
        /*
        _setcolor(normal_col);
        _rectangle(_GBORDER,0,0,639,349);
        */
        SetWin(main);
        BorderWin();

        if (PRINTON) printf("Insert Method Starting...\n");
        p=InsertMethod(d);
        if (PRINTON) printf("Insert Method Complete - Calculating Path Length.\n");

        sound(200);
        delay(500);
        if (p==NULL) {sound(400);delay(500);nosound();}
        nosound();

        length=PathLength(p,d);
        if (PRINTON) printf("Length was %.3f\n",length);
                
        getch();
        
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
