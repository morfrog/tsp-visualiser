#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

void main(int argc,char *argv[])
{
    int num,c;
    double x,y;
    char waste;
    FILE *file=fopen(argv[1],"rb");

    fscanf(file,"%d",&num);
    fgetc(file);
    printf("%d",num);
    for (c=0;c<num;c++)
    {
        fscanf(file,"%f %f",&x,&y);
        printf("%f %f\n",x,y);
        getch();
    }
}
