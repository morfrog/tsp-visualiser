#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <conio.h>
#include <graph.h>
#include <dos.h>
#include <ctype.h>
#include "sys.h"
#include "file.h"
#include "path.h"
#include "screen.h"
#include "problem.h"

void WriteScreen2bmp(void)
{
    static int initialised;
    static int screen_num;
    char filename[13];
    FILE *in;
    FILE *out;
    byte read;
    int x,y;

    if (!initialised)
    {
        strcpy(filename,"VTTSP000.BMP");
        initialised=1;
    }
    in=fopen("VTTSP.DAT","r");
    out=fopen(filename,"w");

    for (x=0;x<1078;x++)
    {
        fread(&read,sizeof(byte),1,in);     // read and write header
        fwrite(&read,sizeof(byte),1,out);
    }
    _setviewport(0,0,639,349);
    for (x=0;x<640;x++)
        for (y=0;y<350;y++)                 // write screen
        {
            read=(byte)_getpixel(x,y);
            fputc(read,out);
        }
}

void WriteScreen2File(void)
{
    static int initialised=0;
    static int screen_num=0;
    static char filename[13];
    FILE *out;
    int x,y;

    if (!initialised)
    {
        strcpy(filename,"VTTSP000.SCR");
        initialised=1;
    }

    filename[7]=48+screen_num;

    if ((out=fopen(filename,"wb"))==NULL)
    {
        ShutDown();
        printf("File Save Error");
        exit(0);
    }
    else
    {
        _setviewport(0,0,639,349);
        for (x=0;x<640;x++)
            for (y=0;y<350;y++)
                fputc(((char) _getpixel(x,y)) ,out);

        fclose(out);
        ++screen_num;
    }
}


int TotalDelay(int tdelay)
{
    static int tot_delay=WAITKEY; // * defaults to getkeypress option * //
    
    if (tdelay==REQUEST)
        return tot_delay;
    else {
        tot_delay=tdelay;
        return -2; // * was set - not request * //
    }
}

void Init2TimeSegs(clock_t *a,clock_t *b)
{
    int y=TotalDelay(REQUEST);
    *a=y/2;
    *b=y;
}

void Init3TimeSegs(clock_t *a,clock_t *b,clock_t *c)
{
    int y=TotalDelay(REQUEST);
    *a=y/3;
    *b=(y*2)/3;
    *c=y;
}

int DoInput(char ch)
// * returns if a change has been made * //
{
    waitresult=WAIT_OK;
    switch (ch) {
        case 27 : {
                    while (kbhit()) getch(); // * clear buffer * //
                    waitresult=WAIT_EXIT;
                    return 0;
                  }
                  break;
        case '0': TotalDelay(0); return 1;
                  break;
        case 13 : TotalDelay(WAITKEY); return 1;
                  break;
        case 'V':
                  Beep(100,100); WriteScreen2File();
                  _setviewport(0,0,460,350); //  <- poxy but works for now
                  break;
        default : if ((ch>=49) && (ch<=57))
                  {
                    TotalDelay((ch-48)*50);
                    return 1;
                  }
                  break;
    }
    return 0;
}

int WaitTill(clock_t it)
// * Waits until the clock() returns >= it * //
{
    if (TotalDelay(REQUEST)<0) { // * if Wait_For_Keypress option * //
        return DoInput(getch());
    }
    else
    {
        do
        {
            if ( kbhit() ) return DoInput(getch());
        }
        while (clock()<it);
    }
    return 0;
}

void DrawCitiesLine(LOCATES *loc,int A,int B,int col)
{
    DrawLine(loc->at[A].x,loc->at[A].y,loc->at[B].x,loc->at[B].y,col);
}

void DrawCityN(LOCATES *loc,int n,double size,int color)
{
    char num[3]="00\0";
    sprintf(num,"%d",loc->at[n].citynum);
    DrawCity(num,loc->at[n].x,loc->at[n].y,
        size,color);

}

void DrawCities(LOCATES *loc,MASK *m)
{
    char num[3]="00\0";
    int count;
    for (count=0;count<(loc->num);count++)
    {
        sprintf(num,"%d",loc->at[count].citynum);
        DrawCity(num,loc->at[count].x,loc->at[count].y,
            m->array[count].size,m->array[count].color);

    }
}

void DrawMaskPath(PATH *p,LOCATES *loc,MASK *m)
{
    int count,a,b,max=p->size;

    PathStart(p);
    for (count=0;count<max;count++)
    {
        PathPair(p,&a,&b);  // get the current path pair
        if (m->array[a].color==m->array[b].color)
            DrawCitiesLine(loc,a,b,m->array[a].color);
        else
            DrawCitiesLine(loc,a,b,line_col);
        PathNext(p);
    }
}

void DrawPath(PATH *p,LOCATES *loc,int returnp)
{
    int count,a,b,max=p->size-(returnp?0:1);

    PathStart(p);
    for (count=0;count<max;count++)
    {
        PathPair(p,&a,&b);  // get the current path pair
        DrawCitiesLine(loc,a,b,line_col);
        PathNext(p);
    }
}

char *DelayStr(int delay,double optimal)
{
    char *buffer=salloc(50,sizeof(char));
    switch (delay) {
        case -1 : sprintf(buffer,"Wait-For-Keypress");              break;
        case 0  : sprintf(buffer,"Run-Through-To-End");             break;
        case -2 : sprintf(buffer,"Benchmark Option");               break;
        case -3 : sprintf(buffer,"All Benchmark Option, Optimal = %10.6f",
                  optimal);                                         break;
        default : sprintf(buffer,"%d Seconds",delay/CLOCKS_PER_SEC);break;
    }
    return buffer;
}

PROBLEM *NewProblem(DISTANCES *dist)
{
    PROBLEM *ret=salloc(1,sizeof(PROBLEM));

    ret->dist=dist;

    ret->path=NULL;
    ret->vmask=NewViewMask(dist->size,city_out,citysize);

    ProblemReset(ret,0);
    return ret;
}

void ProblemReset(PROBLEM *prob,int free)
{
    SetMask(prob->vmask,city_out,citysize);    
    if (free) FreePath(prob->path);
}

double PathLength(PATH *p,DISTANCES *d)
{
    int count;
    double ret=0.0;
    NODE *look=p->head;

    if (p==NULL) return ret;

    for (count=0;count<p->size;count++)
    {
        ret+=d->array[look->citynum][look->next->citynum];
        look=look->next;
    }
    return ret;
}

double InsertCost(PROBLEM *prob,int I, int A, int B)
/* returns cost of inserting I between A and B */
{
    return prob->dist->array[A][I]+
            prob->dist->array[I][B]-
            prob->dist->array[B][A];
}

void BestInsert(PROBLEM *prob,int citynum,int *a,int *b)
{
    int pA,pB,x;
    double thiscost,bestcost;

    PathStart(prob->path);
    PathPair(prob->path,&pA,&pB);
    bestcost=InsertCost(prob,citynum,pA,pB);
    *a=pA;*b=pB;

    PathNext(prob->path);

    for (x=1;x<(prob->path->size);x++)
    {
        PathPair(prob->path,&pA,&pB);
        thiscost=InsertCost(prob,citynum,pA,pB);
        if (thiscost<bestcost)
        {
            bestcost=thiscost;
            *a=pA;*b=pB;
        }
        PathNext(prob->path);
    }
}

void HiliteCityGroup(PROBLEM *prob,int main,int maincol,int city1, int city2,int col)
{
    LOCATES *l=prob->dist->l;
    int A,B,C;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);
    A=SetMaskAt(prob->vmask,maincol,citysize*2,main);
    B=SetMaskAt(prob->vmask,col,citysize*2,city1);
    C=SetMaskAt(prob->vmask,col,citysize*2,city2);

    DrawCities(l,prob->vmask);

    SetMaskAt(prob->vmask,A,citysize,main);
    SetMaskAt(prob->vmask,B,citysize,city1);
    SetMaskAt(prob->vmask,C,citysize,city2);

    DrawLine(l->at[city1].x,l->at[city1].y,l->at[city2].x,l->at[city2].y,col);
    SwapPage();
}

void HiliteCityPair(PROBLEM *prob,int city1,int city2,int col)
{
    LOCATES *l=prob->dist->l;
    int A,B;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,1);

    A=SetMaskAt(prob->vmask,city_hi,citysize,city1);
    B=SetMaskAt(prob->vmask,city_hi,citysize,city2);
    DrawCities(l,prob->vmask);
    SetMaskAt(prob->vmask,A,citysize,city1);
    SetMaskAt(prob->vmask,B,citysize,city2);


    SwapPage();
}

void HiliteCity(PROBLEM *prob,int citynum,int returnp)
{
    LOCATES *l=prob->dist->l;
    int A;
    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,returnp);

    A=SetMaskAt(prob->vmask,city_hi,citysize*2,citynum);
    DrawCities(l,prob->vmask);
    SetMaskAt(prob->vmask,A,citysize*2,citynum);
    SwapPage();
}

void DrawNorm(PROBLEM *prob,int returnp)
{
    LOCATES *l=prob->dist->l;

    _clearscreen(_GVIEWPORT);
    DrawPath(prob->path,l,returnp);
    DrawCities(l,prob->vmask);
}

MASK _FAR *NewViewMask(int size,int color,double csize)
{
    MASK *mask=NULL;

    mask=salloc(1,sizeof(MASK));
    mask->size=size;
    mask->array=salloc(mask->size,sizeof(VNODE));
    SetMask(mask,color,csize);
    return mask;
}

void _FAR SetMask(MASK *m,int color,double size)
{
    int x;
    for (x=0;x<m->size;x++) {
        m->array[x].color=color;
        m->array[x].size=size;
    }
}

void _FAR SetMaskCol(MASK *m,int color)
{
    int x;
    for (x=0;x<m->size;x++) m->array[x].color=color;
}

void _FAR SetMaskSize(MASK *m,double size)
{
    int x;
    for (x=0;x<m->size;x++) m->array[x].size=size;
}

int _FAR SetMaskAt(MASK *m,int color,double size,int pos)
{
    int ret=m->array[pos].color;
    if (color>-1)  m->array[pos].color=color;
    if (size>-1.0) m->array[pos].size=size;
    return ret;
}

void _FAR SetMasksFrom(MASK *m,int *list,int listsize,int col, double size)
{
    int x;
    for (x=0;x<listsize;x++)
    {
        m->array[list[x]].color=col;
        m->array[list[x]].size=size;
    }
}

void _FAR SetMasks(MASK *m,int num, ...) // int pos,int color,double size
{
    va_list ap;
    int x,pos,color;
    double size;

    va_start(ap,num);

    for (x=0;x<num;x++)
    {
        pos=va_arg(ap,int);
        color=va_arg(ap,int);
        size=va_arg(ap,double);
        m->array[pos].color=color;
        m->array[pos].size=size;
    }
    va_end(ap);
}

LOCATES *PathAsLocates(PATH *path,LOCATES *loc)
// * returns loc reordered in path order
// pre : path->size == loc->num
{
    LOCATES *ret=InitLocates(0,loc);
    int x,A;

    PathStart(path);
    for (x=0;x<loc->num;x++) {
        PathCurrent(path,&A);
        ret->at[x].x=loc->at[A].x;
        ret->at[x].y=loc->at[A].y;
        ret->at[x].citynum=loc->at[A].citynum;
        PathNext(path);
    }
    return ret;
}

PATH *LocatesAsPath(LOCATES *loc)
{
    PATH *ret=NewPath(1,loc->at[0].citynum);
    int x;
    for (x=1;x<(loc->num);x++)
        InsertPathEnd(ret,loc->at[x].citynum);
    return ret;
}

void DrawOverlay(PROBLEM *prob,int color,int cities)
{
    LOCATES *l=prob->dist->l;
    SetMask(prob->vmask,color,citysize);
    DrawMaskPath(prob->path,l,prob->vmask);
    SetMask(prob->vmask,city_in,citysize);
    if (cities)  DrawCities(l,prob->vmask);
}

int ControlVarOverlay(PROBLEM *prob,int startx,int starty,int endx,int endy,int col,
                      int l,int t,int r,int b,int (* func)(int,int),int stopwidth,
                      int stopheight,int stoptimes)
// * map overlay to window startx,starty - endx,endy, using l,t,r,b      * // 
// * as increments at each stage of the Overlay transformation, stopping * //
// * when either the func(width,stopwidth) or func(height,stopheight) a  * //
// * -ve value specifying ignore this stopping condition                 * //
{
    SWIN *win=NewWin(startx,starty,endx,endy);
    int times=0;

    while (1)
    {
        SetWin(win);
        _clearscreen(_GVIEWPORT);
        DrawOverlay(prob,col,1);
        SwapPage();
        delay(10);
        ++times;

        if ((stoptimes>=0) && (times>=stoptimes))
            break;
        if ((stopwidth>=0) && ( func((win->r)-(win->l),stopwidth+1) ))
            break;
        if ((stopheight>=0) && ( func((win->b)-(win->t),stopheight+1) ))
            break;

        _clearscreen(_GVIEWPORT);
        MoveWin(win,l,t,r,b);
    }

    _clearscreen(_GVIEWPORT);
    DrawOverlay(prob,col,1);
    SwapPage();
    return 1;
}

int ControlLCLOverlay(PROBLEM *one,PROBLEM *two,int width,int height,int starty,int col1,int col2)
// * Moves overlay from edges to centre to start edges and repeats till esc key hit * //
{
    SWIN *left=NewWin(1,starty,width/2,starty+height-1);
    SWIN *right=NewWin((width/2)+1,starty,width,starty+height-1);
    int factor=1,ld=factor,rd=-factor,edge=0,delayfor=(270)/(one->dist->size);
    int done=0,cx1=1,cy1=starty,cx2=width,cy2=starty+height-1;
    char ch;
    clock_t now;

    ClearBuffer();
    MoveWin(left,ld,0,ld,0);
    MoveWin(right,rd,0,rd,0);

    do
    {
        while (!edge)
        {
            ClearSection(cx1,cy1,cx2,cy2);
            SetWin(left);
            DrawOverlay(one,col1,1);
            SetWin(right);
            DrawOverlay(two,col2,1);
            SwapPage();
            delay(delayfor);
        
            MoveWin(left,ld,0,ld,0);
            MoveWin(right,rd,0,rd,0);

            if (kbhit()) {
                ch=getch();
                while (ch==48) ch=getch(); // * handle pause mode * //
                if (ch==27) return 1;
                if (isdigit(ch))
                {
                    factor=(ch-48); // * = todigit(ch) * //
                    rd=(rd<0?-1:1)*factor;
                    ld=-rd;
                }
            }    

            if (abs((right->l)-(left->l))<factor)
                edge=1;
            else if (((left->l)+factor)>=(right->l)) // centre
                edge=1;
            else if ((left->l)<=factor+1) // edges
                edge=2;
            else if ((rd<0) && (left->l<=factor*2))
                edge=2;
        }
        if (edge==1)
        {
            ClearSection(cx1,cy1,cx2,cy2);
            SetWin(left);
            DrawOverlay(one,col1,0);
            DrawOverlay(two,col2,1);
            SwapPage();
            delay(1000); // * delay for 1 second at middle * // fix to handle 0 keypress for pause
            if (kbhit()) {
                ch=getch();
                while (ch==48) ch=getch(); // * handle pause mode * //
                if (ch==27) return 1;
                if (isdigit(ch))
                {
                    factor=(ch-48); // * = todigit(ch) * //
                    rd=(rd<0?-1:1)*factor;
                    ld=-rd;
                }
            }    

        }
        edge=0; // * not at edge or middle * //
        ld=rd;
        rd*=-1;

        MoveWin(left,ld,0,ld,0);
        MoveWin(right,rd,0,rd,0);
    }
    while (!done);
    return 1;
}

void _FAR ShowText(char string[],int start,int line,int color)
{
    int startpage=_getactivepage();
    _setactivepage(1);
    DrawText(string,start,line,color);
    _setactivepage(0);
    DrawText(string,start,line,color);
    _setactivepage(startpage);
}

/*
void main()
{
    PROBLEM *p=NewProblem(NewMap("30cities.da4"));
    int x,a,b;

    FStartPath(Lesser,p);          
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);
    x=FNextCost(Lesser,p,&a,&b);
    printf("Cheapest is %d, between %d and %d.\n",x,a,b);
    x=FNextCost(Greater,p,&a,&b);
    printf("Dearest is %d, between %d and %d.\n",x,a,b);

    ProblemReset(p,1);
    FStartPath(Greater,p);          
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);

    ProblemReset(p,1);
    RndStartPath(p);
    ShowPath(p->path);
    PrintList(p->outpath,p->endo);
    FNextDist(Lesser,Lesser,p,&x);
    printf("-------\n");

    ProblemReset(p,1);
    getch();

    printf("Val  NN  FN\n");
    for (x=0;x<(p->endo);x++)
        printf("%3d %3d %3d\n",x,FNeighbour(Lesser,p,x),FNeighbour(Greater,p,x));
}
*/


