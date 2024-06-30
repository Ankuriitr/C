#ifndef BASICFN_H
#define BASICFN_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<errno.h>


double average(double* array, int size);
double minimum(double* arr, int size);
double maximum(double* arr, int size);
double pnmean(double* array, int size, int type);
void rmoutliers(double* data, int size, int bound, double rmo_data[], int* rmo_data_size);
double max_array_element(double* array, int start, int arr_end);
void waitFor(unsigned int secs);

#endif
