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

#define InitNearN   -1
#define InitRand     0
#define InitClosest  1
#define InitFurthest 2

#define InsNearN   -1
#define InsRand     0
#define InsCheap    1
#define InsNearest  2
#define InsFurthest 3
#define InsMinEdge  4

#define NoGraphics 0
#define Graphics   1

#define DEBUG 1
#define PRINTON 0

int tot_delay;

void WaitTill(clock_t it)
{
    if (tot_delay<0) getch();
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
        DrawCity(num,loc->at[count].x,loc->at[count].y,citysize,GREY);
    }
}

void DrawPath(PATH *p,LOCATES *loc)
{
    int count,a,b;

    PathStart(p);
    for (count=0;count<(p->size);count++)
    {
        PathPair(p,&a,&b);
        DrawLine(loc->at[a].x,loc->at[a].y,
                 loc->at[b].x,loc->at[b].y,
                 line_col);
        PathNext(p);
    }
}

int ProcessCmdLine(int argc, char *argv[],char **filename,int *InitType,
                    int *InsType,int *tdelay,double *optimal)
{
    int wait_time;
    if (argc<4)
    {
        printf("Visual Tool For The Travelling Salesman Problem.\n");
        printf("Usage: %s filename init method [time].\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
                argv[0],
               "Where   init    in {Random,Furthest,Closest}",
               "        method  in {Random,Cheap,NearN,Nearest,Furthest,MinEdge}",
               "        time    = 0 is calculate to end",
               "        time    = n is wait n seconds between each insert",
               "        time    =-2 is Benchmark Mode - Simply returns length",
               "        time    =-3 double is Benchmark for all methods, double=optimal",
               "        No time = wait for keypress between each insert");

        return 0;
    }
    else
    {
        *filename=argv[1];
        switch (argv[2][0]) { /* Init Type */
            case 'F' : case 'f' : *InitType=InitFurthest; break;
            case 'C' : case 'c' : *InitType=InitClosest; break;
            default : *InitType=InitRand;break;
        }
        switch (argv[3][0]) { /* Insertion Type */
            case 'C' : case 'c' : *InsType=InsCheap; break;
            case 'N' : case 'n' : if ((argv[3][4]=='N') || (argv[3][4]=='n'))
                                      *InsType=InsNearN;
                                  else
                                      *InsType=InsNearest;
                                  break;
            case 'F' : case 'f' : *InsType=InsFurthest; break;
            case 'M' : case 'm' : *InsType=InsMinEdge; break;
            default  :            *InsType=InsRand; break;
        }
        if (argc>=5) /* optional time taken */
        {
            wait_time=atoi(argv[4]);
            if (wait_time>0)
                *tdelay=wait_time*CLOCKS_PER_SEC;
            else if (wait_time<-2)
            {
                *tdelay=-3; /* benchmark only - no graphics, do all, optimal */
                *optimal=(double) atof(argv[5]);
            }
            else if (wait_time<0)
                *tdelay=-2; /* benchmark only - no graphics */
            else
                *tdelay=0;
        }
        else /* optional time not specified */
            *tdelay=-1;
    }
    return 1;
}

void PrintInit(int init)
{
    switch (init) {
        case InitNearN   : printf("NearN    "); break;
        case InitRand    : printf("Random   ");  break;
        case InitClosest : printf("Closest  "); break;
        case InitFurthest: printf("Furthest ");break;
        default :          printf("Illegal Value.");break;
    }
}

void PrintMethod(int method)
{
    switch (method) {
        case InsRand    : printf("Random   ");     break;
        case InsCheap   : printf("Cheapest ");   break;
        case InsNearN   : printf("NNeigbour"); break;
        case InsNearest : printf("Nearest  ");    break;
        case InsFurthest: printf("Furthest ");   break;
        case InsMinEdge : printf("Minimum Edge Distance"); break;
        default         : printf("Illegal Value.");
    }
}

void PrintCmdLine(char *filename,int InitType,int InsType,int delay,int optimal)
{
    printf("Running With Parameters :\nFilename   : %s\nInitType   : ",filename);
    PrintInit(InitType);
    printf("\nInsType    : ");
    PrintMethod(InsType);
    printf("\nDelay Type : ");
    switch (delay) {
        case -1 : printf("Wait-For-Keypress");  break;
        case 0  : printf("Run-Through-To-End"); break;
        case -2 : printf("Benchmark Option");   break;
        case -3 : printf("All Benchmark Option, Optimal = %10.6f",optimal);
        default : printf("%d Seconds",delay/CLOCKS_PER_SEC);break;
    }
    printf("\n");
   
}

void HiliteCityGroup(PROBLEM *prob,int main,int maincol,int city1, int city2,int col)
{
    LOCATES *l=prob->dist->l;
    char num[3]="00\0";
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l);
    DrawCities(l);

    sprintf(num,"%d",l->at[main].citynum);
    DrawCity(num,l->at[main].x,l->at[main].y,citysize*2,maincol);
    sprintf(num,"%d",l->at[city1].citynum);
    DrawCity(num,l->at[city1].x,l->at[city1].y,citysize*2,col);
    sprintf(num,"%d",l->at[city2].citynum);
    DrawCity(num,l->at[city2].x,l->at[city2].y,citysize*2,col);
    DrawLine(l->at[city1].x,l->at[city1].y,l->at[city2].x,l->at[city2].y,col);
    BorderWin();
    SwapPage();
}


void HiliteCityPair(PROBLEM *prob,int city1,int city2,int col)
{
    LOCATES *l=prob->dist->l;
    char num[3]="00\0";
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l);
    DrawCities(l);

    sprintf(num,"%d",l->at[city1].citynum);
    DrawCity(num,l->at[city1].x,l->at[city1].y,citysize*2,col);
    sprintf(num,"%d",l->at[city2].citynum);
    DrawCity(num,l->at[city2].x,l->at[city2].y,citysize*2,col);

    BorderWin();
    SwapPage();
}

void HiliteCity(PROBLEM *prob,int citynum)
{
    LOCATES *l=prob->dist->l;
    char num[3]="00\0";
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l);
    DrawCities(l);
    sprintf(num,"%d",l->at[citynum].citynum);
    DrawCity(num,l->at[citynum].x,l->at[citynum].y,citysize*2,YELLOW);
    BorderWin();
    SwapPage();
}

void DrawNorm(PROBLEM *prob)
{
    LOCATES *l=prob->dist->l;
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l);
    DrawCities(l);
    BorderWin();
    SwapPage();
}

double ControlRand(PROBLEM *prob,int graphics)
{
    int next,Ia,Ib;
    clock_t now;
    clock_t time_seg=tot_delay/3;
    do {
        now=clock();
        next=RndNext(prob);

        if (graphics) {HiliteCity(prob,next);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);

        if (graphics) {DrawNorm(prob);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlNearN(PROBLEM *prob,int graphics)
{
    int x,this,next,first;
    clock_t now;
    clock_t time_seg=tot_delay/2;

    PathStart(prob->path);
    PathCurrent(prob->path,&first);
    for (x=1;x<(prob->path->size);x++) PathNext(prob->path);
    PathCurrent(prob->path,&this); /* the last element in the path */

    do {
        now=clock();
        next=FNeighbour(Lesser,prob,this);

        if (graphics) {HiliteCity(prob,next);WaitTill(now+time_seg);}

        CityDel(prob,next);
        InsertPath(prob->path,next,this,first);
        this=next;

        if (graphics) {DrawNorm(prob);WaitTill(now+(2*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlFurthest(PROBLEM *prob,int graphics)
{
    int next,Ia,Ib,it;
    clock_t now;
    clock_t time_seg=tot_delay/3;

    do {
        now=clock();
        next=FNextDist(Greater,Lesser,prob,&it);

        if (graphics) {HiliteCity(prob,next);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);

        if (graphics) {DrawNorm(prob);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlNearest(PROBLEM *prob,int graphics)
{
    int next,Ia,Ib,it;
    clock_t now;
    clock_t time_seg=tot_delay/3;

    do {
        now=clock();
        next=FNextDist(Lesser,Lesser,prob,&it);

        if (graphics) {HiliteCity(prob,next);WaitTill(now+time_seg);}

        CityDel(prob,next);
        BestInsert(prob,next,&Ia,&Ib);

        if (graphics) {HiliteCityGroup(prob,next,YELLOW,Ia,Ib,BLUE);WaitTill(now+(2*time_seg));}

        InsertPath(prob->path,next,Ia,Ib);

        if (graphics) {DrawNorm(prob);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

double ControlCheap(PROBLEM *prob,int graphics)
{
    int next,pA,pB;
    clock_t now;
    clock_t time_seg=tot_delay/3;

    do {
        now=clock();
        next=FNextCost(Lesser,prob,&pA,&pB);

        if (graphics) {HiliteCity(prob,next);WaitTill(now+(time_seg));}
        if (graphics) {HiliteCityGroup(prob,next,YELLOW,pA,pB,BLUE);WaitTill(now+(2*time_seg));}
        CityDel(prob,next);
        InsertPath(prob->path,next,pA,pB);

        if (graphics) {DrawNorm(prob);WaitTill(now+(3*time_seg));}

    } while (prob->endo);
    return PathLength(prob->path,prob->dist);
}

void main(int argc, char *argv[])
{
    PROBLEM *prob=NULL;
    SWIN *main=NewWin(1,1,350,460);
    double length;

    char *filename;double optimal;
    int inittype,instype,tdelay;

    int init,method;

    if (ProcessCmdLine(argc,argv,&filename,&inittype,&instype,&tdelay,&optimal))
    {
        tot_delay=tdelay;
        if (PRINTON) PrintCmdLine(filename,inittype,instype,tdelay,optimal);

        if (PRINTON) printf("Reading Map From File '%s'...\n",filename);
        prob=NewProblem(NewMap(filename));
        if (PRINTON) printf("Map Read...\n");
        if (PRINTON) PrintMap(prob->dist);
        if (PRINTON) mygetch();
        if (PRINTON) printf("Delay = %d \n",tdelay);


        if (tdelay==-3) { /* all benchmark with optimal - no graphics */

            printf("Init      File/Insert-> ");
            for (method=InsRand;method<=InsFurthest;method++)
                {PrintMethod(method);printf(" ");}
            printf("\n");

            for (init=InitRand;init<=InitFurthest;init++)
            {
                PrintInit(init);
                printf("%12s",filename);
                for (method=InsRand;method<=InsFurthest;method++)
                {
                    switch (init) {
                        case InitClosest : FStartPath(Lesser,prob);  break;
                        case InitFurthest: FStartPath(Greater,prob); break;
                        default : RndStartPath(prob);             break;
                    }

                    switch (method) {
                        case InsCheap   : length=ControlCheap(prob,NoGraphics);break; 
                        case InsNearN   : length=ControlNearN(prob,NoGraphics);break;
                        case InsNearest : length=ControlNearest(prob,NoGraphics);break;
                        case InsFurthest: length=ControlFurthest(prob,NoGraphics);break;
                        case InsMinEdge : break;
                        default/*Rand*/ : length=ControlRand(prob,NoGraphics); break;
        
                    }

                    printf("%10.5f",length/optimal);
                    ProblemReset(prob,1);
                }
                printf("\n");
            }
            
            printf("NearN    ");printf("%12s",filename);
            NNeighbourStartPath(prob);
            length=ControlNearN(prob,NoGraphics);
            printf("%10.5f\n",length/optimal);

        }
        else
        {
            if (method==InsNearN) NNeighbourStartPath(prob);
            else
            {
                switch (inittype) {
                    case InitClosest : FStartPath(Lesser,prob);  break;
                    case InitFurthest: FStartPath(Greater,prob); break;
                    default : RndStartPath(prob);             break;
                }
            }
            if (tdelay==-2) { /* benchmark only - no graphics */
            
                switch (instype) {
                    case InsCheap   : length=ControlCheap(prob,NoGraphics);break; 
                    case InsNearN   : length=ControlNearN(prob,NoGraphics);break;
                    case InsNearest : length=ControlNearest(prob,NoGraphics);break;
                    case InsFurthest: length=ControlFurthest(prob,NoGraphics);break;
                    case InsMinEdge : break;
                    default/*Rand*/ : length=ControlRand(prob,NoGraphics); break;
            
                }

                printf("Length was %.5f\n",length);
            }
            else
            {
                InitScreen(16);
                SwapPage();
    
                SetWin(main);
        
                DrawNorm(prob);
                delay(500);
    
                switch (instype) {
                    case InsCheap   : ControlCheap(prob,((tdelay!=0) && Graphics)); break; 
                    case InsNearN   : ControlNearN(prob,((tdelay!=0) && Graphics)); break;
                    case InsNearest : ControlNearest(prob,((tdelay!=0) && Graphics));break;
                    case InsFurthest: ControlFurthest(prob,((tdelay!=0) && Graphics));break;
                    case InsMinEdge : break;
                    default/*Rand*/ : ControlRand(prob,((tdelay!=0) && Graphics)); break;
        
                }
                DrawNorm(prob);
                getch();

                ShutDown();
            }


                

        }
        

/*        FreeWin(main);    */  
     
/*        FreeMap(d);     */  

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
