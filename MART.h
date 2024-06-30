#ifndef MART_H
#define MART_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>

#include "basicfn.h"

void MART(double* W, double* proj, int proj_size, int ndet, int nrot, double accuracy, double u, int mart_num, double recon[]);

#endif