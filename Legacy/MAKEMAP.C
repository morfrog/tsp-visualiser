#include <stdio.h>
#include <stdlib.h>

double DRand(double num)
{
    return ((double)rand() / (double)RAND_MAX) * num;
}

void main(int argc,char *argv[])
{
    int num,x;
    
    if (argc<2)
        printf("Usage: %s num_of_points\n"
               "Outputs to screen, can use redirection '>' to send to file",
               argv[0]);
    else
    {
        num=atoi(argv[1]);
        fprintf(stdout,"%d\n",num);

        for (x=0;x<num;x++)
        {
            fprintf(stdout,"%8.6f %8.6f\n",DRand(1.0),DRand(1.0));
        }
    }
}
