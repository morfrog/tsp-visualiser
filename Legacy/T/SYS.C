#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

void *salloc(size_t something) {
/**********************************************************************
Description   : Allocates memory, displays an error if there isn't enough
Pre-Condition : The amount of memory to allocate is passed in.
Post-Condition: A pointer to this memory is returned.
**********************************************************************/
    void *p=malloc(something);
    if (p==NULL)
    {
        printf("Out Of Memory Error.\n\n\nGoodBye.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}
