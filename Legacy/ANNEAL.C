#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <time.h>
#include <i86.h>
#include <graph.h>
#include <math.h>
#include <conio.h>
#include <string.h>
#include "sys.h"
#include "file.h"
#include "screen.h"
#include "path.h"
#include "problem.h"
#include "optim.h"
#include "anneal.h"

// #define DEBUG 1
#define topsize 58

char *AnnealStr(int opt)
{
    char *ret=salloc(6,sizeof(char));
    switch (opt) {
        case opt1 : strcpy(ret,"1 Opt"); break;
        case opt2 : strcpy(ret,"2 Opt"); break;
        case opt3 : strcpy(ret,"3 Opt"); break;
    }
    return ret;
}

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

int MethodMaxSize(int method,int max)
{
    switch (method) {
        case opt1 : return max*(max-2); break;
        case opt2 : /*return 1+(((max-2)/2)*max)-(max%2==0?max/2:0); */
                    return PlusFactorial(max-2)-1; break;
        case opt3 : return 9; break;
        default   : printf("Invalid Method");exit(0); break;
    }
    return 0;
}


ANNPROB *NewAnnProb(DISTANCES *dist,int method)
{
    ANNPROB *ret=salloc(1,sizeof(ANNPROB));
    int max=MethodMaxSize(method,dist->size),mmax,x;

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

    max=MethodMaxSize(annprob->method,annprob->prob->dist->size);

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
            if (!(Same(A,B,C,D)))
            {
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
                ++cur_ann;            
            }
            PathNext(prob->path);

        }

        TmpPathIns(prob->path);
#ifdef DEBUG
        ShowPath(prob->path);
#endif
    }
}

void AnnealTwoOpt(ANNPROB *annprob)
{
    PROBLEM *prob=annprob->prob;
    int *array=PathToArray(prob->path,1);
    int max=prob->path->size; // * 1 = extension * //
    int mA,mB,mC,mD,x,y,cur_ann=0;
    double change,best=0.0;

    for (x=0;x<max-2;x++)
        for (y=x+2;y<max;y++)
        {
            mA=array[x];mB=array[x+1];
            mC=array[y];mD=array[y+1];

            if (mA!=mD) { 
                change=prob->dist->array[mA][mC]+prob->dist->array[mB][mD]
                      -prob->dist->array[mA][mB]-prob->dist->array[mC][mD];

                annprob->ann[cur_ann].array[0]=mA;
                annprob->ann[cur_ann].array[1]=mB;
                annprob->ann[cur_ann].array[2]=mC;
                annprob->ann[cur_ann].array[3]=mD;
                annprob->ann[cur_ann].deltaE=change;
            
                ++cur_ann;
            }
        }
    // printf("%d",cur_ann);
    /*s*/free(array);
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
    int x,max=MethodMaxSize(annprob->method,annprob->prob->dist->size);
 
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
    int x,max=MethodMaxSize(annprob->method,annprob->prob->dist->size);
    double change,Of;

    Of=annprob->ann[max-1].SumOfPs; // * the sum of all of the P's * //

    change=(((double)rand())/((double)RAND_MAX))*(Of+1.0);
    
#ifdef DEBUG
    printf("Random = %8.6f ",change);
#endif

    if (change>Of)
        return -1;
    else
    {
        for (x=0;x<max;x++)
        {
            if (annprob->ann[x].SumOfPs>change) break;
        }
    return x;
    }
}

double MakeAnnealChange(ANNPROB *annprob,int change)
{
    int I,A,B,C,D;

    switch(annprob->method) {
    case opt1 :
        I=annprob->ann[change].array[0];
        A=annprob->ann[change].array[3];
        B=annprob->ann[change].array[4];

        DeletePath(annprob->prob->path,I); 
        InsertPath(annprob->prob->path,I,A,B);
        return annprob->ann[change].deltaE;
        break;
    case opt2 :
        A=annprob->ann[change].array[0];
        B=annprob->ann[change].array[1];
        C=annprob->ann[change].array[2];
        D=annprob->ann[change].array[3];

        MakeTwoOptTransition(annprob->prob,A,B,C,D);
        return annprob->ann[change].deltaE;
        break;
    case opt3 :
        break;
    }
    return 0;
}

void ControlAnneal(ANNPROB *one,ANNPROB *two,double temperature,double cooling,int changes)
{
    double temp=temperature;

    double len1=PathLength(one->prob->path,one->prob->dist);
    double len2=PathLength(two->prob->path,two->prob->dist);
    // * calculating both in case of pre-changes for some method or different files * //

    int x,change1,change2,good1,good2;
    int *array1,*array2;
    clock_t now;
    clock_t seg1,seg2,seg3;
    char buffer[50];
    int max1=MethodMaxSize(one->method,one->prob->dist->size);
    int max2=MethodMaxSize(two->method,two->prob->dist->size);
    int nulls1=0,nulls2=0,done1=0,done2=0;

    Init3TimeSegs(&seg1,&seg2,&seg3);

    while ((temp>0.0000005) && (!( (done1) && (done2) )) )
    {
        nulls1=0;nulls2=0;
        for (x=0;x<changes;x++)
        {
            now=clock();
            AnnWinManager(text_top);
            sprintf(buffer,"%3d",x+1);
            AnnShowText(buffer,58,3,LTRED);

            if (!done1)
            {
                AnnealOneOpt(one);
                ApplyTemperature(one,temp);
                change1=ChooseChange(one);
                if (change1==-1)
                    {++nulls1;good1=0;}
                else good1=1;
                AnnWinManager(win_one);
                if (good1) {
                    array1=one->ann[change1].array;
                    HiliteOneOpt(one->prob,array1[1],array1[0],array1[2],hilite_1,1);
                }
                else DrawNorm(one->prob,1);
            }

            if (!done2)
            {
                AnnealTwoOpt(two);
                ApplyTemperature(two,temp);
                change2=ChooseChange(two);
                if (change2==-1)
                    {++nulls2;good2=0;}
                else good2=1;
                AnnWinManager(win_two);
                if (good2) {
                    array2=two->ann[change2].array;
                    HiliteTwoOpt(two->prob,array2[0],array2[1],array2[2],array2[3],0);
                }
                else DrawNorm(two->prob,1);
            }
            SwapPage();
            if (WaitTill(now+seg1)) Init3TimeSegs(&seg1,&seg2,&seg3);
    
            if (!done1)
            {
                AnnWinManager(win_one);
                if (good1) {
                    HiliteOneOpt(one->prob,array1[3],array1[0],array1[4],hilite_2,0);
                    len1+=MakeAnnealChange(one,change1);
                    AnnWinManager(text_one);
                    sprintf(buffer,"%f",len1);
                    AnnShowText(buffer,8,1,LTRED);
                }
            }
            if (!done2)
            {
                AnnWinManager(win_two);
                if (good2) {
                    HiliteTwoOpt(two->prob,array2[0],array2[1],array2[2],array2[3],1);
                    len2+=MakeAnnealChange(two,change2);
                    AnnWinManager(text_two);
                    sprintf(buffer,"%f",len2);
                    AnnShowText(buffer,8,1,LTRED);
                }
            }
            SwapPage();
            if (WaitTill(now+seg2)) Init3TimeSegs(&seg1,&seg2,&seg3);

            AnnWinManager(win_one);
            DrawNorm(one->prob,1);
            AnnWinManager(win_two);
            DrawNorm(two->prob,1);
            SwapPage();
            if (WaitTill(now+seg3)) Init3TimeSegs(&seg1,&seg2,&seg3);

        }
        if (nulls1>=changes) done1=1;
        if (nulls2>=changes) done2=1;
        if (!((done1) && (done2))) {
            temp*=cooling;
            AnnWinManager(text_top);
            sprintf(buffer,"%11.6f",temp);
            AnnShowText(buffer,13,2,LTRED);
        }
        AnnWinManager(text_one);
        sprintf(buffer,"%8.3f",done1?0:100*(double)(changes-nulls1)/changes);
        AnnShowText(buffer,28,2,LTRED);
        AnnWinManager(text_two);
        sprintf(buffer,"%8.3f",done2?0:100*(double)(changes-nulls2)/changes);
        AnnShowText(buffer,28,2,LTRED);

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

    if (!init) {
        texttop=NewWin(1,1,639,topsize-1);
        one=NewWin(1,1+topsize,320,240+topsize);
        textone=NewWin(1,242+topsize,320,349);
        two=NewWin(321,1+topsize,639,240+topsize);
        texttwo=NewWin(321,242+topsize,639,349);
        BorderWin(texttop,border_col);
        BorderWin(textone,border_col);
        BorderWin(texttwo,border_col);
        BorderWin(one,border_col);
        BorderWin(two,border_col);
        init=1;

    }
    switch (which) {
        case win_one  : SetWin(one);  break;
        case win_two  : SetWin(two);  break;
        case text_one : SetTextWin(textone);  break;
        case text_two : SetTextWin(texttwo);  break;
        case text_top : SetTextWin(texttop);  break;
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

 
