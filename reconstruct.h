#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>

#include "MART.h"
#include "w_matrix.h"


void reconstruct(int NR, int NT, double* proj, int proj_size, int method, double rec_image_arr[]);

#endif