

#include "reconstruct.h"


// Reconstructed image in the form of single array. To access array[i][j] element use array[i*colmn+j]. 
void reconstruct(int NR,int NT,double* proj,int proj_size, int method, double rec_image_arr[])
{
    // Generating the Weight matrix.
    int beam = 1;                                                      // Select 1 for parallel beam and otherwise for fanbeam.
    double screen_width = 28.0;                                           // Pixels are located in the screen.
    double det_emit_dis = 24.0;
    double det_arr_width = 24.0;                                          // Detector array width.
    double emit_arr_width = 30.0;                                         // Emitter array width.
    int row = NR * NT;
    //int proj_size = NT * NR;
    int npixel = NT;
    int size_recon = npixel * npixel;
    int colmn = pow(npixel, 2); 
    int size_L = row * colmn;

    // To store the W-Matrix data.
    double* W = (double*)malloc(size_L * sizeof(double));
    if (!W)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(W);
        return -1;
    }

    // To store the reconstruction data.
    double* recon = (double*)malloc(size_recon * sizeof(double));
    if (!recon)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(recon);
        return -1;
    }

    if (method == 1 || method == 2 || method == 3)
    {
        w_matrix(beam, NT, NR, screen_width, det_emit_dis, det_arr_width, emit_arr_width, W);  // To create W matrix.
        
        /*for (int a = 0; a < size_L; a++)
        {
            //printf("W= %lf", W[a]);
            W1[a] = W[a];
        }
        */
        double accuracy = 0.00001;
        double u = 0.8;
        MART(W, proj, proj_size, NT, NR, accuracy, u, method, recon);

        /*
        for (int m = 0; m < npixel; m++)
        {
            for (int n = 0; n < npixel; n++)
            {
                //fprintf(recon_w, "%.10e \t", recon[m * n + n]);
                printf("recon=%lf \n", recon[m * n + n]);
            }
            //fprintf(recon_w, "\n");
        }*/

    }
    else
    {
        printf("Radon under development.");
    }
    for (int i = 0; i < size_recon; i++)
    {
        rec_image_arr[i] = recon[i];
    }
    free(W);
    free(recon);
}

