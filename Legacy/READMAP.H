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

LOCATES *GetLocates();
void ShowLocates(LOCATES *l);

#endif
