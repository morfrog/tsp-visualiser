#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <time.h>
#include <i86.h>
#include <graph.h>
#include <math.h>
#include <conio.h>
#include "sys.h"
#include "file.h"
#include "screen.h"
#include "path.h"
#include "problem.h"
#include "optim.h"
#include "anneal.h"

#define DEBUG 1


int MethodArraySize(int method)
{
    switch (method) {
        case opt1 : return 5; break;
        case opt2 : return 4; break;
        case opt3 : return 9; break;
        default   : printf("Invalid Method");exit(0); break;
    }
    return 0;
}

ANNPROB *NewAnnProb(DISTANCES *dist,int method)
{
    ANNPROB *ret=salloc(1,sizeof(ANNPROB));
    int max=dist->size,mmax,x;

    max*=max-1;

    ret->prob=NewProblem(dist);
    ret->method=method;
    
    ret->ann=salloc(max,sizeof(ANNODE));

    mmax=MethodArraySize(ret->method);
    
    for (x=0;x<max;x++)
        ret->ann[x].array=salloc(mmax,sizeof(int));

    StartRandom();
    AnnProbReset(ret,0);
    return ret;
}

void AnnProbReset(ANNPROB *annprob,int free)
{
    int x,y,max,mmax;

    max=annprob->prob->dist->size;
    max=max*(max-1);

    mmax=MethodArraySize(annprob->method);

    for (x=0;x<max;x++)
    {

        annprob->ann[x].deltaE=0.0;
        annprob->ann[x].P=0.0;
        annprob->ann[x].SumOfPs=0.0;
        for (y=0;y<mmax;y++)
            annprob->ann[x].array[y]=0;
    }
    
    ProblemReset(annprob->prob,free);
}

void AnnealOneOpt(ANNPROB *annprob)
// * sets up the anneal prob array * //
{

    PROBLEM *prob=annprob->prob;
    int *path=PathToArray(prob->path,0); /* list of all to check */
    int max=prob->path->size;
    int insert,pathlook,cur_ann=0,A,B,C,D,I;
    double removal,insertion;

    for (insert=0;insert<max;insert++) // * run thru all cities * //
    {
        I=path[insert];
        TmpPathDel(prob->path,I,&A,&B);
        removal=(prob->dist->array[A][B])
               -(prob->dist->array[I][A])
               -(prob->dist->array[I][B]);

        PathStart(prob->path);
        for (pathlook=0;pathlook<max-1;pathlook++)
        {
            PathPair(prob->path,&C,&D);
            insertion=InsertCost(prob,I,C,D);
#ifdef DEBUG
            printf("I=%d A=%d B=%d",I,A,B);
#endif
            annprob->ann[cur_ann].array[0]=I;
            annprob->ann[cur_ann].array[1]=A;
            annprob->ann[cur_ann].array[2]=B;
            annprob->ann[cur_ann].array[3]=C;
            annprob->ann[cur_ann].array[4]=D;
            annprob->ann[cur_ann].deltaE=insertion+removal;
            
            PathNext(prob->path);
            ++cur_ann;
        }

        TmpPathIns(prob->path);
#ifdef DEBUG
        ShowPath(prob->path);
#endif
    }
}

double GetP(double deltaE,double temperature)
{
    if (deltaE<=0.0)
        return 1.0;
    else
        return exp(0.0 - (deltaE / temperature));
}
    
void ApplyTemperature(ANNPROB *annprob,double temp)
{
    int x,max=annprob->prob->dist->size;
    max=max*(max-1);
 
    annprob->ann[0].P=GetP(annprob->ann[0].deltaE,temp);
    annprob->ann[0].SumOfPs=annprob->ann[0].P;
    for (x=1;x<max;x++)
    {
        annprob->ann[x].P=GetP(annprob->ann[x].deltaE,temp);
        annprob->ann[x].SumOfPs=annprob->ann[x].P+annprob->ann[x-1].SumOfPs;
    }
}

int ChooseChange(ANNPROB *annprob)
// * Returns the randomized change to make from the array of possible changes
// * based on their P values.
{
    int x,max=annprob->prob->dist->size;
    double change,Of;

    max=max*(max-1);
    Of=annprob->ann[max-1].SumOfPs; // * the sum of all of the P's * //

    change=(((double)rand())/((double)RAND_MAX))*Of;
    
#ifdef DEBUG
    printf("Random = %8.6f ",change);
#endif

    for (x=0;x<max;x++)
    {
        if (annprob->ann[x].SumOfPs>change) break;
    }
    return x;
}

void MakeAnnealChange(ANNPROB *annprob,int change)
{
    int I,A,B;

    switch(annprob->method) {
    case opt1 :
        I=annprob->ann[change].array[0];
        A=annprob->ann[change].array[3];
        B=annprob->ann[change].array[4];

        DeletePath(annprob->prob->path,I); 
        InsertPath(annprob->prob->path,I,A,B);
        break;
    case opt2 :
        break;
    case opt3 :
        break;
    }
}

void ControlAnneal(ANNPROB *one,ANNPROB *two,double temperature,double cooling,int changes)
{
    double temp=temperature;
    int x,change;
    int *array;

    while (temp>0.0)
    {
        for (x=0;x<changes;x++)
        {
            AnnealOneOpt(one);
            ApplyTemperature(one,temp);
            change=ChooseChange(one);
            array=one->ann[change].array;

            HiliteOneOpt(one->prob,array[1],array[0],array[2],BLUE,1);
    
            HiliteOneOpt(one->prob,array[3],array[0],array[4],PURPLE,0);

            MakeAnnealChange(one,change);
        }
        temp*=cooling;
    }
}

void _FAR AnnWinManager(int which)
{
    static int init=0;
    static SWIN *one;
    static SWIN *two;
    static SWIN *textone;
    static SWIN *texttwo;
    static SWIN *texttop;
    static int topsize=40;

    if (!init) {
        texttop=NewWin(1,1,640,topsize);
        one=NewWin(1,1+topsize,320,240+topsize);
        textone=NewWin(1,241+topsize,321,350);
        two=NewWin(321,1+topsize,640,240+topsize);
        texttwo=NewWin(321,241+topsize,640,350);
        init=1;
    }
    switch (which) {
        case win_one  : SetWin(one);  break;
        case win_two  : SetWin(two);  break;
        case text_one : SetTextWin(textone,1);  break;
        case text_two : SetTextWin(texttwo,1);  break;
        case text_top : SetTextWin(texttop,1);  break;
    }
}

void _FAR AnnShowText(char string[],int start,int line,int color)
{
    int startpage=_getactivepage();
    _setactivepage(1);
    DrawText(string,start,line,color);
    _setactivepage(0);
    DrawText(string,start,line,color);
    _setactivepage(startpage);
}

 
