#ifndef OPTIM_H

int NextOneOpt(PROBLEM *prob,int *next,int *oA,int *oB,int *nA,int *nB);
double ControlOneOpt(PROBLEM *prob,int graphics);
int NextTwoOpt(PROBLEM *prob,int *A,int *B,int *C,int *D);
double ControlTwoOpt(PROBLEM *prob,int graphics);
void Test();
#endif
