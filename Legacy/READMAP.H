#ifndef READMAP_H
extern struct point{
        double  x,y;
        int citynum;
};

typedef struct point POINT;

struct locates
{
        int num;
        POINT *at; /* array of locations */
};

typedef struct locates LOCATES;

struct matrix
{
        int size; /* <=big */
        int **array;
};

typedef struct matrix DISTANCES;

LOCATES *GetLocates();
void ShowLocates(LOCATES *l);
DISTANCES *LocToDist(LOCATES *l);
void FreeDist(DISTANCES *d);

#endif
