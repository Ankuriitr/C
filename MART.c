/*Reconstruction using MART
* 
Date: 07/02/2022
MART - 1, 2, 3
This code perform 2D reconstruction based on the weight matrix and the
pojection data.

Weight matrix w(i, j); i be the ith rayand j be the jth pixel.
w(i, j)x(j) = B(i); x be the attenuation coefficientsand B be the projection
data.
Steps:-
i.initial guess of the x(j) value.
ii.correction to the x(j) value.
iii.Application of the correction.
iv.Testing the convergence.

@Author = ANKUR
Divyadrishti imaging lab, IIT Roorkee.
ankur89587@gmail.com; ankur@ph.iitr.ac.in
*/

#include "MART.h"



void MART(double* W,double* proj, int proj_size, int ndet, int nrot, double accuracy, double u, int mart_num,double recon[])
{
	/*
	for (int i = 0; i < proj_size; i++)
	{
		//printf("proj[%d]= %lf \n", i, proj[i]);
	}
	*/

/*INPUT
* W have Npixel(total no. of pixels=NT*NT) columns and NT*NR(ndet*nrot) rows. 
proj = np.random.rand(9, 18)
mart_num = 3
ndet = 9
nrot = 18
accuracy = 0.0001                   #Accuracy / Convergence criterion.
u = 0.1                             #Relaxation factor.
max_iter = 100
W = weight matrix        // In the form of 1D array.
*/
	int max_iter = 100;
	int N_pixel = ndet * ndet;             //Total number of pixels.
	int M = ndet * nrot;

	// Initialising correction matrix.
	double* corrtn = (double*)malloc(M * sizeof(double));
	if (!corrtn)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(corrtn);
		return -1;
	}

	double* pnew = (double*)malloc(M * sizeof(double));  //Array top store nW_matrix elements.
	if (!pnew)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(pnew);
		return -1;
	}

	double* fnew = (double*)malloc(N_pixel * sizeof(double));  //The reconstructed image.
	if (!fnew)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(fnew);
		return -1;
	}
       
	int iteration = 0;
	double conv_factor = 1.0;

	double* fold = (double*)malloc(N_pixel * sizeof(double));  //The reconstructed image.
	if (!fold)
	{
		printf(stderr, "Unable to allocate memory. \n");
		free(fold);
		return -1;
	}
	// Initialising fnew and fold with zeros.
	for (int i = 0; i < M; i++)
	{
		corrtn[i] = 0.0;
		pnew[i] = 0.0;
	}
	// Initialising fnew and fold with ones.
	for (int i = 0; i < N_pixel; i++)
	{
		fold[i] = 1.0;
		fnew[i] = 1.0;
	}
	// exon = 1
	int count = 0;
	while ((conv_factor > accuracy) && (iteration < max_iter))
	{
		for (int i = 0; i < M; i++)
		{
			pnew[i] = 0;
			for (int j = 0; j < N_pixel;j++)
			{
				//printf("before fnew= %lf \n", fnew[i]);
				pnew[i] = pnew[i] + W[i * N_pixel + j] * (fnew[j]);
				//printf("fnew= %lf \n", fnew[i]);
			}
			/*
			if (proj_rows == 1)
			{
				proj = transpose(proj);
			}
			*/
			if (pnew[i] != 0)
			{
				corrtn[i] = proj[i] / pnew[i];
			}
			else
			{
				corrtn[i] = 0;
			}		

			if (mart_num == 1)
			{
				for (int j = 0; j < N_pixel;j++)
				{
					if (W[i * N_pixel + j] != 0)
					{
						fnew[j] = fnew[j] * (1 - u * (1 - corrtn[i]));
					}					
					else
					{
						fnew[j] = fnew[j];
					}						
				}
			}
			if (mart_num == 2)
			{
				// fnew(j, 1) = fnew(j, 1) * ((c(i, 1)). ^ (lamda * w(i, j) / max(w(i, :))));     //LENT MART
				// fnew(j, 1) = fnew(j, 1) * (1 - lamda * w(i, j) * (1 - c(i, 1)) / W(i, 1));

				for (int j = 0; j < N_pixel; j++)
				{
					double max_W = max_array_element(W, i * N_pixel, (i + 1) * N_pixel);
					if (max_W != 0)
					{
						fnew[j] = fnew[j] * pow((corrtn[i]), (u * W[i * N_pixel + j] / max_W));
					}						
					else
					{
						fnew[j] = fnew[j];
					}						
				}
			}


			if (mart_num == 3)
			{
				for (int j = 0; j < N_pixel; j++)
				{
					if (W[i * N_pixel + j] != 0)
					{
						fnew[j] = fnew[j] * pow((corrtn[i]), (u * W[i * N_pixel + j]));              //K_MART
						//fnew[j, 0] = fnew[j, 0] * (1 - u * W[i * N_pixel + j] * (1 - corrtn[i, 0]) / max(W[i, ]))   //M_MART
					}
					else
					{
						fnew[j] = fnew[j];
					}

				}

			}
		}
		// Initialising array to store the error.
		double* error = (double*)malloc(N_pixel * sizeof(double));
		if (!error)
		{
			printf(stderr, "Unable to allocate memory. \n");
			free(error);
			return -1;
		}
		for (int k = 0; k < N_pixel; k++)
		{
			error[k] = fabs(fnew[k] - fold[k]) / fnew[k];
		}	
		conv_factor = 100 * (minimum(error,N_pixel));
		free(error);

		for (int a = 0; a < N_pixel; a++)
		{
			fold[a] = fnew[a];
			//printf("fold=%lf \n", fold[a]);
		}	
		iteration = iteration + 1;
	}
	for (int i = 0; i < N_pixel; i++)
	{
		recon[i] = fnew[i];
	}

	free(corrtn);
	free(pnew);
	free(fnew);
	free(fold);
}



