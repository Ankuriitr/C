#ifndef GNU_PLOT_H
#define GNU_PLOT_H

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

void gnuplot_open();
void plot_graph(double* array1, double* array2, int size, char* xlabel, char* ylabel, char* title);
void plot_matrix(int terminal, double* array, int size1, int size2, char* title);
void gnuplot_close();

#endif