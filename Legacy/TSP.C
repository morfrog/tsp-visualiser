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

// #define DEBUG 1
// #define PRINTON 0

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
               "        method  in {Random,Cheap,Dear,NearN,Nearest,Furthest,N2,F2}\n"
               "        time    = 0 is calculate to end\n"
               "        time    = n is take n 10ths of seconds for each insert\n"
               "        time    =-2 is Benchmark Mode - Simply returns length\n"
               "        time    =-3 optimal is Benchmark for all methods,\n"
               "                               with optimal calculation\n"
               "        No time = wait for keypress between each insert\n");
        printf("E.G. %s myfile.in Random Dear",argv[0]);

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
                                  else if (argv[3][1]=='2')
                                      *InsType=InsNearest2;  
                                  else
                                      *InsType=InsNearest;
                                  break;
            case 'F' : case 'f' : if (argv[3][1]=='2')
                                    *InsType=InsFurthest2; 
                                  else
                                    *InsType=InsFurthest;
                                  break;
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
    printf("Running With Parameters :\n"
           "Filename   : %s\n"
           "InitType   : %s\n"
           "InsType    : %s\n"
           "Delay Type : %s\n",filename,InitStr(InitType),MethodStr(InsType),
                               DelayStr(delay,optimal));
}

void main(int argc, char *argv[])
{
    INSPROB *insprob=NULL;
    LOCATES *out=NULL;
    double length;

    char *filename;double optimal;
    int inittype,instype,tdelay;

    int init,method;

    char buffer[60];

    if (ProcessCmdLine(argc,argv,&filename,&inittype,&instype,&tdelay,&optimal))
    {
        TotalDelay(tdelay);
#ifdef PRINTON
    PrintCmdLine(filename,inittype,instype,tdelay,optimal);
    printf("Reading Map From File '%s'...\n",filename);
#endif
        insprob=NewInsProb(NewMap(NewLocates(filename)));
#ifdef PRINTON
            printf("Map Read...\n");
            PrintMap(insprob->prob->dist);
            mygetch();
            printf("Delay = %d \n",tdelay);
#endif        

        if (tdelay==-3) { /* all benchmark with optimal - no graphics */

            printf("Init      File/Insert-> ");
            for (method=InsRand;method<=InsFurthest2;method++)
                {printf("%s",MethodStr(method));printf(" ");}
            printf("\n");

            for (init=InitRand;init<=InitFurthest;init++)
            {
                printf("%s",InitStr(init));
                printf(" %12s",filename);
                for (method=InsRand;method<=InsFurthest2;method++)
                {
                    length=ControlAllIns(insprob,init,method,NoGraphics);
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
        else if (tdelay==-2) { /* benchmark only - no graphics */

                length=ControlAllIns(insprob,inittype,instype,NoGraphics);
                printf("Length was %.5f\n",length);
        }
        else
        {

            InitScreen(16);
            SwapPage();

            WinManager(Main);
            WinManager(Text);

            ShowText("Hit Esc to Quit,\n\n"
                 "Any other key \n"
                 "To continue..",1,21,BLUE);


            length=ControlAllIns(insprob,inittype,instype,tdelay!=0);
        
            DrawNorm(insprob->prob,1);
            SwapPage();

            sprintf(buffer,"F I N I S H E D");
            ShowText(buffer,1,16,LTRED);
            sprintf(buffer,"Length Is %8.6f",length);
            ShowText(buffer,1,18,YELLOW);

            ShowText("                \n"
                     "Hit Esc         \n"
                     "to Quit...   \n"
                     "               ",1,19,LTRED);
            
            while (getch()!=27) {}
            ShutDown();
        }

/*        FreeWin(main);    */  
     
/*        FreeMap(insprob->prob->dist);     */

        out=PathAsLocates(insprob->prob->path,insprob->prob->dist->l);
        WriteLocates(out,"default.dat");
/*        FreeLocates(out);*/
        if (tdelay>=-1) printf("File 'default.dat' written...\n");
    }

}

