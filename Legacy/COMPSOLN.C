#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <graph.h>
#include "sys.h"
#include "file.h"
#include "path.h"
#include "screen.h"
#include "problem.h"

#define VERSION 1.01

int ProcessCmdLine(int argc,char *argv[],char **fileone,char **filetwo)
{
    if (argc<3) {
        printf("Visualisation Tool For The Travelling Salesman Problem\n"
               "Solution Comparison. V%3.2f\n",VERSION);
        printf("Usage: %s file1 file2\n",argv[0]);
        printf("E.G. %s optimal.res mymethod.res",argv[0]);
        return 0;
    }
    else
    {
        *fileone=argv[1];
        *filetwo=argv[2];
        return 1;
    }
}

void main(int argc,char *argv[])
{
    char *fileone,*filetwo,ch;
    PROBLEM *one,*two;
    char buffer[50];
    if (ProcessCmdLine(argc,argv,&fileone,&filetwo))
    {
        one=NewProblem(NewMap(NewLocates(fileone)));
        one->path=LocatesAsPath(one->dist->l);

        two=NewProblem(NewMap(NewLocates(filetwo)));
        two->path=LocatesAsPath(two->dist->l);

        
        InitScreen(16);
    
        SwapPage();
        WinManager(Main);
        WinManager(Text);
        DrawNorm(two,1);
        DrawNorm(one,1);
        
        sprintf(buffer,"Viewing %s",filetwo);
        DrawText(buffer,1,5,GREEN);
        sprintf(buffer,"Length %6.5f",PathLength(two->path,two->dist));
        DrawText(buffer,1,7,YELLOW);
        SwapPage();
        sprintf(buffer,"Viewing %s",fileone);
        DrawText(buffer,1,5,GREEN);
        sprintf(buffer,"Length %6.5f",PathLength(one->path,one->dist));
        DrawText(buffer,1,7,YELLOW);
        
        sprintf(buffer,"Solution Comparison\n"
                       "        of\n"
                       "%s and %s",fileone,filetwo);
        ShowText(buffer,1,1,RED);

        ShowText("Hit Esc to Quit,\n\n"
                 "Any other key \n"
                 "to continue..",1,9,BLUE);

        do {
            SwapPage();
            // Beep(100,1);
            ch=getch();
        } while (ch!=27);
       // FreeProblem(one);
       // FreeProblem(Two);
        ShutDown();
    }
    


}
