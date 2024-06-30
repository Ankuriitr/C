#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>


void maxx(double* arr, int size, double* maximum, int* location);
// data= input dataset.
// size= number of elements in data.
// distance= proximity to find peaks.
// threshold= set threshold to find peaks.
void findpeaks(double* data, size_t size,int distance, double peaks[], int location[]);

#endif
