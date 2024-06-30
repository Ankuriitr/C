#ifndef PROJ_DTRND_H
#define PROJ_DTRND_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<errno.h>

#include "polyfit.h"
#include "polyval.h"
#include "basicfn.h"
#include "findpeaks.h"

void proj_dtrnd(char* filename, int N_packets, double** data, int data_size, int trend, double* proj);


#endif