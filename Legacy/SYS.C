#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <conio.h>
#include <time.h>
#include <dos.h>

void *salloc(int n_items,size_t something) {
/**********************************************************************
Description   : Allocates memory, displays an error if there isn't enough
Pre-Condition : The amount of memory to allocate is passed in.
Post-Condition: A pointer to this memory is returned.
**********************************************************************/
    void *p=NULL;
    if ( (p=malloc(n_items*something)) == 0 )
    {
        printf("Out Of Memory Error.\n\n\nGoodBye.\n");
        exit(EXIT_FAILURE);
    }
    /*
    else
    {
        printf("%d Memory Allocated",_msize(p) / something);
        if ((_msize(p) / something)!=n_items) printf(":UNEXPECTED - Required %d.",n_items);
        printf("\n");
    }
    */   
    return p;
}

char *
allocate_array(int nelements, int sizeof_element, char *array_name)
{
    char    *p=NULL;
    int i;


    if ( (p=salloc(nelements,sizeof_element)) == 0 )
    {
        printf("allocate_array: error in salloc() for array '%s'\n",array_name);
        exit(1);
    }

    for ( i=0 ; i<nelements*sizeof_element ; ++i ) p[i] = 0;

    return(p);

} /* allocate_array */

char *
allocate_N_by_N_array(int n1, int n2, int sizeof_element, char *array_name)
{
    char    **p=NULL;
    int i;


    p = (char **) allocate_array(n1, sizeof(char *), array_name);

    for ( i=0; i<n1 ; ++i )
    {
        p[i] = allocate_array(n2, sizeof_element, array_name);
    }

    return( (char *) p);

} /* allocate_N_by_N_array */

int sfree(void *ptr)
{
    if (ptr==NULL)
        return 0;
/*    else
        free(ptr);*/
    return 1;

}

char mygetch()
{
    printf("Hit any key...\n");
    return getch();
}

void StartRandom()
{
    struct dostime_t time;
    _dos_gettime(&time);
    srand(((time.hsecond)*(time.second))%RAND_MAX);
}

int Random(int num)
{
    return (rand() % num); /* 0..num-1 */
}

void PrintList(int *list,int listsize)
{
    int x;
    for (x=0;x<listsize;x++)
        printf("%2d ",list[x]);
    printf("\n");
}

int PosOf(int num,int *list,int listsize)
/* returns pos of num in unsorted list, -1 of not in list */
{
    int x;
    for (x=0;x<listsize;x++)
        if (list[x]==num) return x;
    return -1;
}

int Lesser(double this,double best)
{
    if (this<best) return 1;
    return 0;
}

int Greater(double this,double best)
{
    if (this>best) return 1;
    return 0;
}


