#include <stdio.h>
#include <stdlib.h>

double DRand(double num)
{
    return ((double)rand() / (double)RAND_MAX) * num;
}

void main(int argc,char *argv[])
{
    int num,x;
    FILE *out=stdout;
    
    if (argc<2)
        printf("Usage: %s num_of_points [file]\n"
               "Outputs to screen,or to optional file\n"
               "E.G. %s 12 myfile.in\n",argv[0],argv[0]);
    else
    {
        if (argc==3) out=fopen(argv[2],"wt");
        num=atoi(argv[1]);
        fprintf(out,"%d\n",num);

        for (x=0;x<num;x++)
        {
            fprintf(out,"%8.6f %8.6f\n",DRand(1.0),DRand(1.0));
        }
        fclose(out);
    }
}
