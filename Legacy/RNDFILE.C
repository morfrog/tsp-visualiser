#include <stdio.h>
#include <stdlib.h>
#include "sys.h"  // Salloc,Random,StartRandom
#include "file.h" // NewLocates

void rndfile(char *infile,char *outfile)
{
    LOCATES *inloc=NULL,*outloc=NULL;
    int *array,x,pos,max,tmp,times;

    inloc=NewLocates(infile);
    outloc=InitLocates(0,inloc);
    max=inloc->num;
    array=salloc(max,sizeof(int));

    for (x=0;x<max;x++)
        array[x]=x;
    StartRandom();
    for (times=0;times<2;times++)
    {
        for (x=0;x<max;x++)
        {
            pos=Random(max-x);
            // swap max and pos
            tmp=array[max-1];
            array[max-1]=array[pos];
            array[pos]=tmp;
        }
    }
    for (x=0;x<max;x++)
    {
        outloc->at[x].x=inloc->at[array[x]].x;
        outloc->at[x].y=inloc->at[array[x]].y;
        outloc->at[x].citynum=inloc->at[array[x]].citynum;
    }
    WriteLocates(outloc,outfile);
    FreeLocates(&inloc);
    FreeLocates(&outloc);
}

void main (int argc,char *argv[])
{
    if (argc<3)
    {   
        printf("Usage: %s infile outfile\n"
               "E.G. %s myfile.in myfile.out\n",argv[0],argv[0]);
    }
    else
    {
        printf("Reading File...\n");
        printf("Randomizing...\n");
        rndfile(argv[1],argv[2]);
        printf("Writing File...\n");
    }
}
