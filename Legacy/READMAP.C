#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "readmap.h"

POINT *n;
int size;

void read_points(int pos)
                        /* Position into which this call of read_points
                         * should write. First call gets 0.
                         */
/*
 *  changes pointer "n" to point to array of double numbers
 *  changes int "size" to indicate size of array "n"
 */
{
        double  x, y;
        int     flag;

        flag = fscanf(stdin,"%Lg %Lg\n", &x, &y);
        
        if ( flag == 2 )
        {
            read_points(pos+1);
            n[pos].x = x;
            n[pos].y = y;
            n[pos].citynum = pos;
            return;
        }
        else
        if (( flag == EOF ) || (flag == 0))
        {
            size = pos;
            n = (struct point *)(malloc(sizeof(struct point) * size));
            return;
        }
        else
        {
            printf("%d...",flag);
            fprintf(stderr,"read_points: read error, pos = %d\n", pos);
            exit(1);
        }
        
} /* read_points */

LOCATES *GetLocates()
{
    LOCATES *ret=malloc(sizeof(LOCATES *));

    read_points(0);
    ret->num=size;
    ret->at=n;
    return ret;
}

void ShowLocates(LOCATES *l)
{
    int x,max=l->num;
 
    for (x=0;x<max;x++)
    {
        printf("City %d is (%g,%g)\n",l->at[x].citynum,l->at[x].x,l->at[x].y);
    }
}

/*
void main()
{
    LOCATES *l=GetLocates();
    ShowLocates(l);
}
*/
