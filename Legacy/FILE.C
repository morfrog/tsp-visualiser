#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include <ctype.h>
#include "sys.h"
#include "file.h"

// #define PRINTON 1

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

int IsFileChar(char c)
{
    return 1; // all chars are file chars
}

int IsIntChar(char c)
{
    return (isdigit(c) || (c=='+') || (c=='-'));
}

int IsDoubleChar(char c)
{
    return ((isdigit(c)) || (c=='.') || c=='-' || c=='+' || c=='E' || c=='e');
}

int ReadLines(FILE *f,int lines,char *buffer)
{
    int linesread=0;
    int pos=0;
    char ch;
    while (linesread<lines)
    {
        ch=fgetc(f);
//        printf("%c",ch);
        if (ch=='\n') ++linesread;
        buffer[pos]=ch;
        ++pos;
    }
    return pos-1;
}

double _FAR ReadDouble(FILE *f)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       Reads an int from a file, removing leading non
 *                              {-+Ee0123456789.} chars and stopping at an
 *                              EOF or space or new line character       
 *      Pre             :       the file exists
 *      Post    :       the integer is returned, the file pointer is past the 
 *                              integer
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

{       
    char ch;
    char buffer[20]="                   \0";
    int x=0;
    ch=fgetc(f);
    if (!IsDoubleChar(ch)) /* find first char of double */
        do ch=fgetc(f); while (!IsDoubleChar(ch));

    while (IsDoubleChar(ch))
    {
        buffer[x]=ch;
        ch=fgetc(f);
        ++x;
        if (x==20) {
            printf("ReadDouble: number exceeds 20 char limit");
            exit(0);
        }
    }
    if (ch=='\n') ungetc(ch,f);
    return atof(buffer);
}

double _FAR Dist(POINT a,POINT b)
{
    double y=(b.y-a.y);
    double x=(b.x-a.x);
    /* printf("%.4f,%.4f to %.4f,%.4f is %.4f %.4f.\n",a.x,a.y,b.x,b.y,x,y);*/
    return sqrt((x*x) + (y*y));

}

LOCATES *InitLocates(int size,LOCATES *iloc)
// if loc==NULL inits to empty of size size, else inits as copy of loc
{
    LOCATES *loc=NULL;
    int x;

    loc=salloc(1,sizeof(LOCATES));
    loc->num=(iloc==NULL)?size:(iloc->num);      /* the number of points */
    loc->at=salloc(loc->num,sizeof(POINT));
    
    if (loc!=NULL)
    {
        for (x=0;x<loc->num;x++)
        {
            loc->at[x].x=iloc->at[x].x;
            loc->at[x].y=iloc->at[x].y;
            loc->at[x].citynum=iloc->at[x].citynum;
        }
    }
    // else dont init to anything

    return loc;
}

int WriteLocates(LOCATES *loc,char *filename)
{
    FILE *output=fopen(filename,"wt");
    int x;
    
    if (output==NULL) return 0;
    fprintf(output,"%d\n",loc->num);
    for (x=0;x<loc->num;x++)
        fprintf(output,"%8.7f %8.7f\n",loc->at[x].x,loc->at[x].y);
    fclose(output);
    return 1;
}

LOCATES *NewLocates(char *filename)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      Desc    :       reads map locations in from a file
 *      Pre     :       none
 *      Post    :       exits program if file not found else returns the
 *                      locations
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
{
    LOCATES *loc=NULL;
    int count,size;

    FILE *input=fopen(filename,"rt");
    if (input==NULL)
    {
        printf("Can't find file \"%.12s\", Exiting System.\n",filename);
        mygetch();
        exit(EXIT_FAILURE);
        return NULL;
    }
    else
    {
        size=(int)ReadDouble(input);
        loc=InitLocates(size,NULL);
        SkipLine(input);
        
        for (count=0;count<loc->num;count++) 
        {
            (loc->at[count]).x=ReadDouble(input);
            (loc->at[count]).y=ReadDouble(input);
            (loc->at[count]).citynum=count;
            SkipLine(input);
        }
#ifdef PRINTON
    ShowLocates(loc);
    printf("loc->num=%d\n",loc->num);
    mygetch();
#endif
        fclose(input);
        return loc;
    }

}

DISTANCES *NewMap(LOCATES *loc)
// * Converts a location array to a distances matrix, assigns distance
// * matrix' locates as loc.

{
    DISTANCES *dist=NULL;

    int count,count2,n;
    double ndist;

    dist=salloc(1,sizeof(DISTANCES));
    dist->size=loc->num;      /* the number of points */
    dist->array=
    allocate_N_by_N_array(dist->size,dist->size,sizeof(double),"Distance");        
#ifdef PRINTON
    printf(" %.4f\n",dist->size);
#endif

    n=(loc->num);
    for (count=0;count<n;count++) 
    {
        for (count2=count;count2<n;count2++) 
        {
            // printf("%.2f %.2f<%.2f ",count,count2,n);
            
            ndist=Dist(loc->at[count],loc->at[count2]);

            // printf("Dist= %.2f.\n",ndist);
            
            dist->array[count][count2]=ndist;
            dist->array[count2][count]=ndist;
        }
    }
    dist->l=loc;
    return dist; 
}

int _FAR FreeLocates(LOCATES **loc)
{
    int ret=1;
    if (!(sfree((*loc)->at))) {
        printf("Failed To Free Locates 'At' Array.\n");
        ret=0;
    }
    if (!(sfree(*loc))) {
        printf("Failed To Free Locates Structure.\n");
        ret=0;
    }
    return ret;
}

int _FAR FreeMap(DISTANCES **d)
{
    int ret=1;
    if (!FreeLocates( &((*d)->l))) ret=0;
    if (!(sfree((*d)->array))) {
        printf("Failed To Free Distances Array.\n");
        ret=0;
    }
    if (!(sfree(*d))) {
        printf("Failed To Distances Structure.\n");
        ret=0;
    }
    return ret;
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

