#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define UnitRand ((double)rand()) /((double)RAND_MAX)

void main(int argc,char *argv[])
{
    int x,num=atoi(argv[1]);
    struct dostime_t time;

    if (argc<2)
    {
        fprintf(stdout,"ERROR: Command Line City Count not supplied (int).\n");
    }
    else 
    {
        if (argc==3) srand(atoi(argv[2]));
        else
        {
            _dos_gettime(&time);
            srand(((time.hsecond)*(time.second))%RAND_MAX);
        }
        for (x=0;x<num-1;x++)
        {
            fprintf(stdout,"%5.5g %5.5g\n",UnitRand,UnitRand);
        }
    }
    fprintf(stdout,"%5.5g %5.5g%c",UnitRand,UnitRand,EOF);
}
