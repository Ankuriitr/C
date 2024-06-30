#ifndef W_MATRIX_H
#define W_MATRIX_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>

#include "basicfn.h"

void w_matrix(int beam, int NT, int NR, double screen_width, double det_emit_dis, double det_arr_width, double emit_arr_width, double W[]);

#endif
