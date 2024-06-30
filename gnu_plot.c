#include "gnu_plot.h"


FILE* gnuplotPipe = NULL;

void gnuplot_open() 
{
	gnuplotPipe=_popen(" E:\\Ankur\\Clang\\gnuplot\\bin\\gnuplot", "w");	
}


void plot_graph( double* array1, double* array2, int size,char* xlabel,char* ylabel,char* title)
{
	fprintf(gnuplotPipe, " set terminal qt\n");
	//fprintf(gnuplotPipe, " cd 'E:\\Ankur\\Clang\\OpenGL_practice\\GNU_plot_P1'\n");
	//fprintf(gnuplotPipe, " plot \"temp.dat\" u 1:2 w lines,");
	fprintf(gnuplotPipe, " set xlabel '%s'\n", xlabel);
	fprintf(gnuplotPipe, " set ylabel '%s'\n", ylabel);
	fprintf(gnuplotPipe, " set title '%s'\n", title);
	fprintf(gnuplotPipe, " plot '-' w lines\n");
	for (int i = 0; i <size; i++) {
		fprintf(gnuplotPipe, "  %f %f\n", array1[i], array2[i]);
		//printf("  %e  %e \n",array1[i], array2[i]);
	}
	fprintf(gnuplotPipe, "  exit\n");
	fflush(gnuplotPipe);
}

void plot_matrix(int terminal, double* array, int size1, int size2,char* title)
{
	fprintf(gnuplotPipe, " set terminal qt %d\n", terminal);
	fprintf(gnuplotPipe, " set autoscale xfix\n");
	fprintf(gnuplotPipe, " set autoscale yfix\n");
	fprintf(gnuplotPipe, " set autoscale cbfix\n");
	fprintf(gnuplotPipe, " set title '%s'\n", title);
	fprintf(gnuplotPipe, " plot '-' matrix with image notitle\n");
	/*
	// Creating temporary file to plot matrix.
	FILE* fp1 = NULL;
	errno_t err5 = fopen_s(&fp1, "temp.dat", "w");
	if (!fp1)
	{
		printf("FILE proj_write NOT OPENED. \n");
		exit(0);
	}*/
	for (int i = 0; i < size1; i++) {
		for (int j = 0; j < size2; j++)
		{
			fprintf(gnuplotPipe, "%e  ", array[i * size2 + j]);
			//fprintf(gnuplotPipe, "  %e %e \n", time[nlocs[i]], x[nlocs[i]]);
		}
		fprintf(gnuplotPipe, " \n");
	}
	//fprintf(gnuplotPipe, " plot 'temp.dat' matrix with image notitle \n");

	//fclose(fp1);
	fprintf(gnuplotPipe, " exit\n");
	fflush(gnuplotPipe);
	//remove("temp.dat") == 0;
} 

void gnuplot_close() 
{

	fflush(gnuplotPipe);
	fprintf(stderr, " Press enter to continue...");
	fflush(stderr);
	getchar();
	// exit gnuplot
	fprintf(gnuplotPipe,"  exit\n");
	_pclose(gnuplotPipe);
}