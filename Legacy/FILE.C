#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include "sys.h"
#include "file.h"

/*void main()
{
        DISTANCES *d=NewMap("file.in");
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


int _FAR ReadInt(FILE *f)
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
    return atoi(buffer);
}

int _FAR Dist(POINT a,POINT b)
{
    int num;
    long y=(b.y-a.y);
    long x=(b.x-a.x);
    printf("%d,%d to %d,%d is %d %d.\n",a.x,a.y,b.x,b.y,(int)x,(int)y);
    num=(int)(sqrt((double)(x*x + y*y)));
    if (num<0) return (num*-1);
    else return num;

}

DISTANCES _FAR *NewMap(char *filename)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       reads a new map in from a file
 *      Pre             :       none
 *      Post    :       exits program if file not found else returns the map
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{
    DISTANCES *dist=salloc(sizeof(DISTANCES *));
    LOCATES loc;

    FILE *input=fopen(filename,"rt");
    int count,count2,n;
    char type[12]="            "; 
    char name[STR_LEN];
    int ndist;

    if (input==NULL)
    {
        printf("Can't find file \"%.12s\", Exiting System.",filename);
        getch();
        exit(EXIT_FAILURE);
        return NULL;
    }
    else
    {
        printf("Reading File %s...\n",filename); 
        fgets(type,12,input);
        printf("%s",type);
        SkipLine(input);
        dist->size=ReadInt(input);                      /* the number of points */
        printf(" %d\n",dist->size);
        SkipLine(input);
        if (strcmp(type,"Geographic")) /* is Geographic */
        {     
            loc=salloc(sizeof(LOCATES *));
            loc->num=(dist->size);
            loc->at=salloc(loc->num*sizeof(POINT *));
            for (count=0;count<loc->num;count++) 
            {
                (loc->at[count]).x=ReadInt(input);
                (loc->at[count]).y=ReadInt(input);
                (loc->at[count]).citynum=count;
                SkipLine(input);

            }
            for (count=0;count<loc->num;count++)
            {
                printf("Point (%d,%d)\n",(loc->at[count]).x,(loc->at[count]).y);
            }
            printf("loc->num=%d\n",loc->num);
            n=(loc->num);
            getch();
            for (count=0;count<n;count++) 
            {
                for (count2=count;count2<n;count2++) 
                {
                    printf("%d %d<%d ",count,count2,n);
                    ndist=Dist(loc->at[count],loc->at[count2]);
                    printf("Dist= %d.\n",ndist);
/*
                    dist->array[count][count2]=ndist;
                    dist->array[count2][count]=ndist;
*/
  
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
    free(d->l->at);
    free(d->l);
    free(d);
}

void _FAR PrintMap(DISTANCES *d)
{
    int count,count2,size=(d->size);
            
    printf("Map Is...\n");
    for (count=0;count<size;count++) 
    {
        for (count2=0;count2<size;count2++) 
                printf("%2d ",d->array[count2][count]);
        printf("\n");
            
    }
}
