/* Code for generating the weight matrix.
"""
Created on Thu Dec 2 12:19 : 55 2021

@author : Ankur
"""
""" This code is written by Ankur, DD Imaging Lab, IITR.
This code returns the 2D weight matrix for paralleland fanbeam.
""" 
*/

#include "w_matrix.h"


#define pi (4*atan(1))

// the compare function for double values
static int cmppos(const void* a, const void* b)
{
    if (*(double*)a > *(double*)b) return 1;
    else if (*(double*)a < *(double*)b) return -1;
    else return 0;
}
// the compare function for double values
static int cmpneg(const void* a, const void* b)
{
    if (*(double*)a > *(double*)b) return -1;
    else if (*(double*)a < *(double*)b) return 1;
    else return 0;
}

void w_matrix(int beam, int NT, int NR, double screen_width,double det_emit_dis,double det_arr_width,double emit_arr_width,double W[])
{   
    /*
    int beam = 1;                                                      // Select 1 for parallel beam and otherwise for fanbeam.
    int NR = 30;                                                       // No. of detector= no. of linear translation for single det. UCT.
    int NT = 30;
    double screen_width = 12.0;                                           // Pixels are located in the screen.
    double det_emit_dis = 15.0;
    double det_arr_width = 10.0;                                          // Detector array width.
    double emit_arr_width = 10.0;                                         // Emitter array width.
    */

    // Initialisisng the variables.
    double fanbeam_angle = 45.0;                                        // Fanbeam angle. 
    double max_rot = 360.0;
    double start_angle = 0.0;
    double theta = ((max_rot - start_angle) / NT) * pi / 180;
    double theta_max = max_rot * pi / 180;
    int npixel = NT;                                      // Number of pixels in each array.

    //// Initialising the arrays.
    // Source initial coordinates array.
    double* sx = (double*)malloc(NT * sizeof(double));      
    if (!sx)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(sx);
        return -1;
    }

    double* sy = (double*)malloc(NT * sizeof(double));      
    if (!sy)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(sy);
        return -1;
    }
    // Detector initial coordinates array.
    double* dx = (double*)malloc(NT * sizeof(double));
    if (!dx)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(dx);
        return -1;
    }
    double* dy = (double*)malloc(NT * sizeof(double));
    if (!dy)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(dy);
        return -1;
    }
    // pixel coordinates array.
    int pixel_array_size = NT + 1;

    double* px = (double*)malloc(pixel_array_size * sizeof(double));
    if (!px)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(px);
        return -1;
    }
    double* py = (double*)malloc(pixel_array_size * sizeof(double));
    if (!py)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(py);
        return -1;
    }

    // screen and detectors position coordinates calculation based on the beam type.
    // For PARALLEL beam  
    if (beam == 1)
    {
        for (int i = 0; i < NR; i++)
        {
            //       print('i= \t',i)                            
            sx[i] = -det_emit_dis / 2;                             // x coordinates of source.
        }
        for (int i = 0; i < NR; i++)
        {
            sy[i] = emit_arr_width / 2 - i * emit_arr_width / (NR - 1);
        }
        for (int i = 0; i < NR; i++)
        {
            dx[i] = det_emit_dis / 2;
        }
        for (int i = 0; i < NR; i++)
        {
            dy[i] = det_arr_width / 2 - i * det_arr_width / (NR - 1);
        }
    }

    // For FANBEAM.                                                              
    else
    {
        for (int i = 0; i < NR; i++)
        {
            sx[i] = -det_emit_dis / 2;
            sy[i] = 0;
        }
        for (int i = 0; i < NR; i++)
        {
            dx[i] = det_emit_dis * cos((fanbeam_angle / 2 - i * fanbeam_angle / (NR - 1)) * pi / 180.0) + sx[i];
        }

        for (int i = 0; i < NR; i++)
        {
            dy[i] = det_emit_dis * sin((fanbeam_angle / 2 - i * fanbeam_angle / (NR - 1)) * pi / 180.0);
        }
    }
    double xpixel = screen_width / npixel;
    double ypixel = xpixel;
    px[0] = -screen_width / 2;                                     // dividing into two parts such that centre lie in the middle point.
    for (int i = 1; i < pixel_array_size; i++)                             // Finding the coordinates of pixels. 
    {
        px[i] = px[0] + i * xpixel;
    }
    py[0] = -screen_width / 2;
    for (int i = 1; i < pixel_array_size; i++)                             // Finding the coordinates of pixels. 
    {
        py[i] = py[0] + i * ypixel;
    }
            
    double yul = py[npixel];                                           // upper limit of y coordinates.
    double yll = py[0];                                                 // Lower limit of y coordinates.
    double xul = px[npixel];                                            // upper limit of x coordinates.   
    double xll = px[0];                                                 // Lower limit of x coordinates.
    int row = NR * NT;
    int colmn = pow(npixel, 2);
    int size_L = row * colmn;
    // defining the size of the final distance matrix.
    // Defining one D array and accessing it as 2D.
    /* L[row][col]=L[row*col] 
    * To access L[i][j]=L[i*col+j].
    * 
    */
    double* L = (double*)malloc(size_L * sizeof(double));
    if (!L)
    {
        printf(stderr, "Unable to allocate memory. \n");
        free(L);
        return -1;
    } 
    // Intialising wmatrix with zeroes.
    for (int m = 0; m < size_L; m++)
    {
        L[m] = 0.0;
    }
    //L = np.zeros([row, colmn]);                                   
    int rloop = 0;
    for (float r = 0.0; r <= theta_max - theta / 2.0; r = r + theta)          //For loop for rotations.
    {
        rloop = rloop + 1;
        // print('r= \t',r)
        // angle=180/pi*r;
        // Array to store the intersection points.
        double* intx = (double*)malloc(pixel_array_size * sizeof(double));    //pixel_array_size=NT+1=npixel+1. 
        if (!intx)
        {
            printf(stderr, "Unable to allocate memory. \n");
            free(intx);
            return -1;
        }
        double* inty = (double*)malloc(pixel_array_size * sizeof(double));    //pixel_array_size=NT+1=npixel+1. 
        if (!inty)
        {
            printf(stderr, "Unable to allocate memory. \n");
            free(inty);
            return -1;
        }
        // Initialising intx and inty with zeros.
        for (int i = 0; i < pixel_array_size; i++)
        {
            intx[i] = 0.0;
            inty[i] = 0.0;
        }
        for (int i = 0; i < NR; i++)                                          //For loop for each detector for each rotation.
        {
            int temp_ar_size = 2 * pixel_array_size;                          //pixel_array_size=NT+1=npixel+1.
            double* xtemp = (double*)malloc(temp_ar_size * sizeof(double));     
            if (!xtemp)
            {
                printf(stderr, "Unable to allocate memory. \n");
                free(xtemp);
                return -1;
            }
            double* ytemp = (double*)malloc(temp_ar_size * sizeof(double));
            if (!ytemp)
            {
                printf(stderr, "Unable to allocate memory. \n");
                free(ytemp);
                return -1;
            }

            double vy = (sy[i] * cos(r) - sx[i] * sin(r));
            double vx = (sx[i] * cos(r) + sy[i] * sin(r));
            double dyyy = (dy[i] * cos(r) - dx[i] * sin(r));
            double dxxx = (dx[i] * cos(r) + dy[i] * sin(r));
            //printf(" vy=%f \t vx=%f \t dyyy=%f \t dxxx=%f \n",vy,vx,dyyy,dxxx);
            int count = 0;
            //finding y coordinates of intersection points.
            for (int j = 0; j <= npixel; j++)
            {
                inty[j] = ((dyyy - vy) / (dxxx - vx)) * (px[j] - vx) + vy;
                if ((inty[j] >= yll) && (inty[j] <= yul))
                {
                    xtemp[count] = px[j];
                    ytemp[count] = inty[j];
                    count++;
                }
            }

            //finding x coordinates on intersection points.
            for (int j = 0; j <= npixel; j++)
            {
                intx[j] = ((dxxx - vx) / (dyyy - vy)) * (py[j] - vy) + vx;
                if ((intx[j] >= xll) && (intx[j] <= xul))
                {
                    ytemp[count] = py[j];
                    xtemp[count] = intx[j];
                    count++;
                }
            }
            int len_ytemp = count;
            /*
            for (int p = 0; p < len_ytemp; p++)
            {
                printf("i=%d \t xtemp =%f \n", i, xtemp[p]);
            }
            */

            if ((dyyy > 0) && (dxxx >= 0))          // Quadrant 1, x and y are positive.
            {
                qsort(ytemp, count, sizeof(double), cmppos);        // Positive sorting, using "cmpneg" comparing fn.
                qsort(xtemp, count, sizeof(double), cmppos);

                for (int j = 0; j < (len_ytemp - 1); j++)
                {
                    for (int s = 0; s < NR; s++)
                    {
                        double lim1 = yul - s * ypixel;
                        double lim2 = yul - (s + 1) * ypixel;
                        if ((ytemp[j] < lim1) && (ytemp[j] >= lim2))
                        {
                            for (int n = 0; n < npixel; n++)
                            {
                                double lim3 = xll + n * xpixel;
                                double lim4 = xll + (n + 1) * xpixel;
                                if ((xtemp[j] >= lim3) && (xtemp[j] < lim4))
                                {
                                    int i1 = npixel * (rloop - 1) + i;
                                    int j1 = npixel * s + n;
                                    L[i1 * colmn + j1] = sqrt(pow((ytemp[j] - ytemp[j + 1]), 2) + pow((xtemp[j] - xtemp[j + 1]), 2)); 
                                    //printf("L1= %f \n", L[i1 * colmn + j1]);
                                }
                            }
                        }
                    }
                }
            }

            if ((dyyy <= 0) && (dxxx > 0))          // Quadrant 2, x  positive and y are negative.
            {
                qsort(ytemp, count, sizeof(double), cmpneg);        // negative sorting, using "cmpneg" comparing fn.
                qsort(xtemp, count, sizeof(double), cmppos);
                for (int j = 0; j < (len_ytemp - 1); j++)
                {
                    for (int s = 0; s < NR; s++)
                    {
                        double lim1 = yul - s * ypixel;
                        double lim2 = yul - (s + 1) * ypixel;
                        if ((ytemp[j] <= lim1) && (ytemp[j] > lim2))
                        {
                            for (int n = 0; n < npixel; n++)
                            {
                                double lim3 = xll + n * xpixel;
                                double lim4 = xll + (n + 1) * xpixel;
                                if ((xtemp[j] >= lim3) && (xtemp[j] < lim4))
                                {
                                    int i1 = npixel * (rloop - 1) + i;
                                    int j1 = npixel * s + n;
                                    L[i1 * colmn + j1] = sqrt(pow((ytemp[j] - ytemp[j + 1]), 2) + pow((xtemp[j] - xtemp[j + 1]), 2));
                                    //printf("L2= %f \n", L[i1 * colmn + j1]);
                                }
                            }
                        }
                    }
                }
            }
            if ((dyyy > 0) && (dxxx <= 0))          // Quadrant 3, x negative and y are positive.
            {
                qsort(ytemp, count, sizeof(double), cmppos);        // Positive sorting, using "cmppos" comparing fn.
                qsort(xtemp, count, sizeof(double), cmpneg);        // Negative sorting, using "cmpneg" comparing fn.
                for (int j = 0; j < (len_ytemp - 1); j++)
                {
                    for (int s = 0; s < NR; s++)
                    {
                        double lim1 = yul - s * ypixel;
                        double lim2 = yul - (s + 1) * ypixel;
                        if ((ytemp[j] < lim1) && (ytemp[j] >= lim2))
                        {
                            for (int n = 0; n < npixel; n++)
                            {
                                double lim3 = xll + n * xpixel;
                                double lim4 = xll + (n + 1) * xpixel;
                                if ((xtemp[j] > lim3) && (xtemp[j] <= lim4))
                                {
                                    int i1 = npixel * (rloop - 1) + i;
                                    int j1 = npixel * s + n;
                                    L[i1 * colmn + j1] = sqrt(pow((ytemp[j] - ytemp[j + 1]), 2) + pow((xtemp[j] - xtemp[j + 1]), 2));
                                    //printf("L3= %f \n", L[i1 * colmn + j1]);
                                }
                            }
                        }
                    }
                }
            }
            else                                                    // Quadrant 4, x negative and y are negative.
            {
                qsort(ytemp, count, sizeof(double), cmpneg);        // Negative sorting, using "cmpneg" comparing fn.
                qsort(xtemp, count, sizeof(double), cmpneg);        // Negative sorting, using "cmpneg" comparing fn.
                for (int j = 0; j < (len_ytemp - 1); j++)
                {
                    for (int s = 0; s < NR; s++)
                    {
                        double lim1 = yul - s * ypixel;
                        double lim2 = yul - (s + 1) * ypixel;
                        if ((ytemp[j] <= lim1) && (ytemp[j] > lim2))
                        {
                            for (int n = 0; n < npixel; n++)
                            {
                                double lim3 = xll + n * xpixel;
                                double lim4 = xll + (n + 1) * xpixel;
                                if ((xtemp[j] > lim3) && (xtemp[j] <= lim4))
                                {
                                    int i1 = npixel * (rloop - 1) + i;
                                    int j1 = npixel * s + n;
                                    L[i1 * colmn + j1] = sqrt(pow((ytemp[j] - ytemp[j + 1]), 2) + pow((xtemp[j] - xtemp[j + 1]), 2));
                                    //printf("L4= %f \n", L[i1 * colmn + j1]);
                                }
                            }
                        }
                    }
                }
            }
            
            free(xtemp);
            free(ytemp);

        }

        free(intx);
        free(inty);

    }
    
    FILE* wmatrix = NULL;
    errno_t err2 = fopen_s(&wmatrix, "wmatrix.dat", "w");
    if (!wmatrix)
    {
        printf("FILE NOT OPENED. \n");
        exit(0);
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < colmn; j++)
        {
            fprintf(wmatrix, " %f \t", L[i * colmn + j]);
            //printf(" W= %lf \t", L[i * colmn + j]);
        }
        fprintf(wmatrix, " \n");
    }
    fclose(wmatrix);
    
    for (int k = 0; k < size_L; k++)
    {
        W[k] = L[k];
        //printf(" W= %lf \t", W[k]);
    }
    //The Weight Matrix in 1D array.
    // To access w[i][j]=w[i*columns+j];


    free(sx);
    free(sy);
    free(dx);
    free(dy);
    free(px);
    free(py);
    free(L);

}

