#include <stdio.h>
#include <stdlib.h>
#include "sys.h"  // Salloc,Random,StartRandom
#include "file.h" // NewLocates

void main (int argc,char *argv[])
{
    char *infile=NULL,*outfile=NULL;
    LOCATES *inloc=NULL,*outloc=NULL;
    int *array,x,pos,max,tmp,times;

    if (argc<3)
    {   
        printf("Usage: %s infile outfile\n"
               "E.G. %s myfile.in myfile.out\n",argv[0],argv[0]);
    }
    else
    {
        infile=argv[1];
        outfile=argv[2];
        
        printf("Reading File...\n");
        inloc=NewLocates(infile);
        outloc=InitLocates(0,inloc);
        
        max=inloc->num;
        array=salloc(max,sizeof(int));

        printf("Randomizing...\n");
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
        printf("Writing File...\n");

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
}
