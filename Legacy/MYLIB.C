#include <stdlib.h>
#include <graph.h>

void *salloc(unsigned Quantity, unsigned sizeof_something) {
/**********************************************************************
Description   : Allocates memory, displays an error if there isn't enough
Pre-Condition : The amount of memory to allocate is passed in.
Post-Condition: A pointer to this memory is returned.
**********************************************************************/
    void *p;
    if ((p = malloc(sizeof_something))==NULL) 
                closegraph();printf("Memory Error");exit(EXIT_FAILURE);
    return p;
}
