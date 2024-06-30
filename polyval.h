#ifndef POLYVAL_H
#define POLYVAL_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>

void polyval(double* coefficients,int poly_degree, double* x_value, int x_size, double pn[]);

#endif
