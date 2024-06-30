// To generate the polynomial PnX^(n-1)for the given coefficients at the given X.

#include "polyval.h"

void polyval(double* coefficients,int order, double* x_value, int x_size, double pn[]) {

	// Order=10 , means there are 11 coefficients.
	//printf("Order =%d \n", order);

	// Loop to calculate the polynomial.
	for (int i = 0; i < x_size; i++)
	{
		double sum = 0.0;
		// Loop to calculate each value.

		//printf("x_value= %e \n", x_value[i]);
		/*
		for (int n = 0; n < order; n++)
		{
			sum += coefficients[n] * pow(x_value[i], (order - n-1));
			//printf("Polynomial= P(%d)*x^(%d) \n", n, (order - n - 1));
			//printf("n= %d \t Sum= %e \n",n, sum);
		}
		*/
		// Horner's method implementation.
		for (int n = 0; n < order; n++)
		{
			//printf("x_value= %e \n", x_value[i]);
			//y = y * x + p[i];
			sum = sum*x_value[i]  + coefficients[n];
			//printf("n= %d \t Sum= %e \n", n, sum);
		}
		
		pn[i] = sum;
	}
}
