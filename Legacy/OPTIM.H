#ifndef OPTIM_H

#define opt1 1
#define opt2 2
#define opt3 3

#define Text 0    // WinManager
#define Main 1    // WinManager

void _FAR OptWinManager(int which);
void _FAR OptShowText(char string[],int start,int line,int color);

int NextOneOpt(PROBLEM *prob,int *next,int *oA,int *oB,int *nA,int *nB,double *improve);
void HiliteOneOpt(PROBLEM *prob,int A,int B,int C,int color,int lines);
double ControlOneOpt(PROBLEM *prob,int graphics);

int NextTwoOpt(PROBLEM *prob,int *A,int *B,int *C,int *D);
void MakeTwoOptTransition(PROBLEM *p,int A,int B,int C,int D);
void HiliteTwoOpt(PROBLEM *prob,int A,int B,int C,int D,int type);
double ControlTwoOpt(PROBLEM *prob,int graphics);

double Control3Opt(PROBLEM *p,int graphics);

double ControlAllOpt(PROBLEM *prob,int method,int graphics,int tdelay);
void Test();
#endif
