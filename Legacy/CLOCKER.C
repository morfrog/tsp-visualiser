#include <stdio.h>
#include <math.h>
#include <time.h>
#include <conio.h>

void main()
{
    clock_t start_time, end_time;
    int x;
    
    for (x=0;x<1;x+=0)
    {
        start_time = clock();
        printf("%lu %lu\n",start_time,start_time / CLOCKS_PER_SEC);
    }
}

_clearscreen
_setactivepage
