#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include "sys.h"
#include "file.h"
 
/*
void main()
{
        int c;
        DISTANCES *d=NewMap("file.in");
        for (c=0;c<10;c++)
        {
            d->array[c][c]=c;
            printf("%.2f is %.2f\n  %s",c,d->array[1][c],
            c==d->array[1][c]?"OK":"Fail...");
        }
        getch();
        printf("Map Read...\n");
        PrintMap(d);
        getch();

}
*/

void _FAR ReadToken(FILE *f,char string[],int max)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Desc    :   Reads a string from a file, removes leading spaces and
 *              stops after max chars or a space or a newline/EOF is reached 
 *  Pre     :   the file exists
 *  Post    :   the string is initialised, null appended to it
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{   int x=0;
    char ch=fgetc(f);
    while ((ch!=' ') && (ch!='\n') && (ch!=EOF) && (x<max)) {
        string[x]=ch;
        ch=fgetc(f);
        ++x;
    }
}

void _FAR SkipTill(FILE *f,char this)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       Skips chars in file until this is encountered, skips it 
 *                              too
 *  Pre         :       the file exists
 *      Post    :       the file pointer is on the char after the first this is 
 *              encountered
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{
    char ch;
    do ch=fgetc(f); while (ch!=this);
}

void _FAR SkipLine(FILE *f)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       Skips a line in a file
 *      Pre             :       file exists and is not at the end of the file
 *      Post    :       the file pointer points to the next line
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{
    SkipTill(f,'\n');
}


double _FAR ReadDouble(FILE *f)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       Reads an int from a file, removing leading spaces and
 *                              stopping at an EOF or space or new line character       
 *      Pre             :       the file exists
 *      Post    :       the integer is returned, the file pointer is past the 
 *                              integer
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

{       
    char ch;
    static char buffer[10]="          ";
    int x=0;
    strcpy(buffer,"          ");
    ch=fgetc(f);
    if (ch==' ') /* remove leading spaces */
        do ch=fgetc(f); while (ch==' ');

    while ((ch!=' ') && (ch!='\n') && (ch!=EOF))
    {
        buffer[x]=ch;
        ch=fgetc(f);
        ++x;
    }
    if (ch=='\n') ungetc(ch,f);
    return atof(buffer);
}

double _FAR Dist(POINT a,POINT b)
{
    double num;
    double y=(b.y-a.y);
    double x=(b.x-a.x);
    printf("%.4f,%.4f to %.4f,%.4f is %.4f %.4f.\n",a.x,a.y,b.x,b.y,x,y);
    return sqrt((x*x) + (y*y));

}

DISTANCES *NewMap(char *filename)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       reads a new map in from a file
 *      Pre             :       none
 *      Post    :       exits program if file not found else returns the map
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{
    DISTANCES *dist=NULL;
    LOCATES *loc=NULL;

    FILE *input=fopen(filename,"rt");
    int count,count2,n;
    char *type=salloc(20,sizeof(char));
/*    char name[STR_LEN];*/
    double ndist;

    if (input==NULL)
    {
        printf("Can't find file \"%.12s\", Exiting System.",filename);
        mygetch();
        exit(EXIT_FAILURE);
        return NULL;
    }
    else
    {
        fgets(type,sizeof(type),input);
        printf("%s",type);
        SkipLine(input);

        dist=salloc(1,sizeof(DISTANCES));
        dist->size=ReadDouble(input);      /* the number of points */
        dist->array=
        allocate_N_by_N_array(dist->size,dist->size,sizeof(double),"Distance");        
        printf(" %.4f\n",dist->size);
        SkipLine(input);
        if (strncmp(type,"Geographic",sizeof(char)*3)==0) /* is Geographic */
        {     
            loc=salloc(1,sizeof(LOCATES));
            loc->num=(dist->size);
            loc->at=salloc(loc->num,sizeof(POINT));
            for (count=0;count<loc->num;count++) 
            {
                (loc->at[count]).x=ReadDouble(input);
                (loc->at[count]).y=ReadDouble(input);
                (loc->at[count]).citynum=count;
                SkipLine(input);

            }
            ShowLocates(loc);
            printf("loc->num=%d\n",loc->num);
            n=(loc->num);
            mygetch();
            for (count=0;count<n;count++) 
            {
                for (count2=count;count2<n;count2++) 
                {
                    printf("%.2f %.2f<%.2f ",count,count2,n);
                    ndist=Dist(loc->at[count],loc->at[count2]);
                    printf("Dist= %.2f.\n",ndist);
                    dist->array[count][count2]=ndist;
                    dist->array[count2][count]=ndist;
                }
            }
            dist->l=loc;
                        
        }
        else
        {
            printf("not defined");

        }
        fclose(input);
        return dist; 
    }
}

void _FAR FreeMap(DISTANCES *d)
{
    if (!(sfree(d->l->at))) printf("Failed To Free Locates 'At' Array.\n");
    if (!(sfree(d->l))) printf("Failed To Free Locates Structure.\n");
    if (!(sfree(d->array))) printf("Failed To Free Distances Array.\n");
    if (!(sfree(d))) printf("Failed To Distances Structure.\n");
}

void _FAR PrintMap(DISTANCES *d)
{
    int count,count2,size=(d->size);
            
    printf("Map Is...\n");
    for (count=0;count<size;count++) 
    {
        for (count2=0;count2<size;count2++) 
                printf("%.2f ",d->array[count2][count]);
        printf("\n");
            
    }
}

void ShowLocates(LOCATES *loc)
{
    int count;
    for (count=0;count<loc->num;count++)
    {
        printf("Point (%.2f,%.2f)\n",(loc->at[count]).x,(loc->at[count]).y);
    }
}

