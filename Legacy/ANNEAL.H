#ifndef ANNEAL_H

#define opt1 1
#define opt2 2
#define opt3 3

/*

    1opt needs point,old1,old2,new1,new2
    2opt needs A,B,C,D
    3opt needs A,B,C,D,E,F,revAB,revCD,revEF
*/

#define win_one  1
#define win_two  2
#define text_one 3
#define text_two 4
#define text_top 5

struct ann_pos
{
    int *array; // size = (1opt=5) (2opt=4) (3opt=9);
    double deltaE;
    double P;  // = e^(-deltaE/T)
    double SumOfPs;
};

typedef struct ann_pos ANNODE;

struct ann_prob
{
    PROBLEM *prob;
    int method;
    ANNODE *ann;  /* array of ANNODE */
};

typedef struct ann_prob ANNPROB;

ANNPROB *NewAnnProb(DISTANCES *dist,int method);
void AnnProbReset(ANNPROB *annprob,int free);
void AnnealOneOpt(ANNPROB *annprob);
void AnnealTwoOpt(ANNPROB *annprob);
void ApplyTemperature(ANNPROB *annprob,double temp);
int ChooseChange(ANNPROB *annprob);
double MakeAnnealChange(ANNPROB *annprob,int change);
void ControlAnneal(ANNPROB *one,ANNPROB *two,double temperature,double cooling,int changes);
int ControlAnnOverlay(ANNPROB *one,ANNPROB *two,int width,int height,int starty);
void _FAR AnnWinManager(int which);
void _FAR AnnShowText(char string[],int start,int line,int color);

#endif

