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
#include "insert.h"

#define VERSION 1.01

#define DEBUG 1
#define PRINTON 0

int ProcessCmdLine(int argc, char *argv[],char **filename,int *InitType,
                    int *InsType,int *tdelay,double *optimal)
{
    int wait_time;
    if (argc<4)
    {
        printf("Visualisation Tool For The Travelling Salesman Problem\n"
               "Insertion Methods. V%3.2f\n",VERSION);
        printf("Usage: %s filename init method [time].\n%s",
                argv[0],
               "Where   init    in {Random,Furthest,Closest}\n"
               "        method  in {Random,Cheap,Dear,NearN,Nearest,Furthest}\n"
               "        time    = 0 is calculate to end\n"
               "        time    = n is take n 10ths of seconds for each insert\n"
               "        time    =-2 is Benchmark Mode - Simply returns length\n"
               "        time    =-3 double is Benchmark for all methods, double=optimal\n"
               "        No time = wait for keypress between each insert\n");

        return 0;
    }
    else
    {
        *filename=argv[1];
        switch (argv[2][0]) { // * Init Type * //
            case 'F' : case 'f' : *InitType=InitFurthest; break;
            case 'C' : case 'c' : *InitType=InitClosest; break;
            default : *InitType=InitRand;break;
        }
        switch (argv[3][0]) { // * Insertion Type * //
            case 'C' : case 'c' : *InsType=InsCheap; break;
            case 'N' : case 'n' : if ((argv[3][4]=='N') || (argv[3][4]=='n'))
                                      *InsType=InsNearN;
                                  else
                                      *InsType=InsNearest;
                                  break;
            case 'F' : case 'f' : *InsType=InsFurthest; break;
            case 'D' : case 'd' : *InsType=InsDear; break;
            default  :            *InsType=InsRand; break;
        }
        if (argc>=5) // * optional time taken * //
        {
            wait_time=atoi(argv[4]);
            switch (wait_time) {
                case 0 : *tdelay=0;  break;
                case -2: *tdelay=-2; break; // * benchmark only - no graphics
                case -3: *tdelay=-3; *optimal=(double) atof(argv[5]); break;
                default: *tdelay=wait_time*(CLOCKS_PER_SEC/10); break;
            }
        }
        else *tdelay=-1; // * optional time not specified * //
            
    }
    return 1;
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

void main(int argc, char *argv[])
{
    INSPROB *insprob=NULL;
    LOCATES *out=NULL;
    double length;

    char *filename;double optimal;
    int inittype,instype,tdelay;

    int init,method;

    char buffer[50];

    if (ProcessCmdLine(argc,argv,&filename,&inittype,&instype,&tdelay,&optimal))
    {
        TotalDelay(tdelay);
        if (PRINTON) PrintCmdLine(filename,inittype,instype,tdelay,optimal);

        if (PRINTON) printf("Reading Map From File '%s'...\n",filename);
        insprob=NewInsProb(NewMap(NewLocates(filename)));
        if (PRINTON) {
            printf("Map Read...\n");
            PrintMap(insprob->prob->dist);
            mygetch();
            printf("Delay = %d \n",tdelay);
        }
        

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
                    if (method==InsNearN)
                    {
                        NNeighbourStartPath(insprob);
                        length=ControlNearN(insprob,NoGraphics);break;
                    }
                    else
                    {
                        switch (inittype) {
                            case InitClosest : FStartPath(Lesser,insprob);  break;
                            case InitFurthest: FStartPath(Greater,insprob); break;
                            default : RndStartPath(insprob);             break;
                        }
                    

                        switch (method) {
                            case InsCheap   : length=ControlCheap(insprob,NoGraphics);break;
                            case InsDear    : length=ControlDear(insprob,NoGraphics);break;
                            case InsNearest : length=ControlNearest(insprob,NoGraphics);break;
                            case InsFurthest: length=ControlFurthest(insprob,NoGraphics);break;
                            default/*Rand*/ : length=ControlRand(insprob,NoGraphics); break;
        
                        }
                    }
                    printf("%10.5f",length/optimal);
                    InsProbReset(insprob,1);
                }
                printf("\n");
            }
            
            printf("NearN    ");printf("%12s",filename);
            NNeighbourStartPath(insprob);
            length=ControlNearN(insprob,NoGraphics);
            printf("%10.5f\n",length/optimal);

        }
        else
        {
            
            if (instype==InsNearN)
            {
                NNeighbourStartPath(insprob);
            }
            else
            {
                switch (inittype) {
                    case InitClosest : FStartPath(Lesser,insprob);  break;
                    case InitFurthest: FStartPath(Greater,insprob); break;
                    default : RndStartPath(insprob);             break;
                }
            }
            if (tdelay==-2) { /* benchmark only - no graphics */
            
                switch (instype) {
                    case InsCheap   : length=ControlCheap(insprob,NoGraphics);break; 
                    case InsDear    : length=ControlDear(insprob,NoGraphics);break; 
                    case InsNearN   : length=ControlNearN(insprob,NoGraphics);break;
                    case InsNearest : length=ControlNearest(insprob,NoGraphics);break;
                    case InsFurthest: length=ControlFurthest(insprob,NoGraphics);break;
                    default/*Rand*/ : length=ControlRand(insprob,NoGraphics); break;
            
                }

                printf("Length was %.5f\n",length);
            }
            else
            {
                InitScreen(16);
                SwapPage();
    
                WinManager(Main);
                WinManager(Text);
        
                DrawNorm(insprob->prob,1);
                delay(tdelay>0?tdelay:0);
    
                switch (instype) {
                    case InsCheap   : length=ControlCheap(insprob,((tdelay!=0) && Graphics)); break;
                    case InsDear    : length=ControlDear(insprob,((tdelay!=0) && Graphics)); break;
                    case InsNearN   : length=ControlNearN(insprob,((tdelay!=0) && Graphics)); break;
                    case InsNearest : length=ControlNearest(insprob,((tdelay!=0) && Graphics));break;
                    case InsFurthest: length=ControlFurthest(insprob,((tdelay!=0) && Graphics));break;
                    default/*Rand*/ : length=ControlRand(insprob,((tdelay!=0) && Graphics)); break;
        
                }
                DrawNorm(insprob->prob,1);
                if (Graphics)
                {
                    sprintf(buffer,"F I N I S H E D");
                    ShowText(buffer,1,16,LTRED);
                    sprintf(buffer,"Length Is %8.6f",length);
                    ShowText(buffer,1,18,YELLOW);
                }
                

                getch();

                ShutDown();
            }


                

        }
        

/*        FreeWin(main);    */  
     
/*        FreeMap(d);     */  

        out=PathAsLocates(insprob->prob->path,insprob->prob->dist->l);
        WriteLocates(out,"default.dat");
        FreeLocates(out);
        if (tdelay>=0) printf("File 'default.dat' written...\n");
    }

}

