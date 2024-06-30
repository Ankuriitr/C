/*This code process the signal to find peak to peak value.
The proj_data values are peak to peak values with outliers removed.
It take the mean of the final amplitudes.
filename: name of the data file
Select trend == 1 if you see significant trend in the data.
** This code can take array of size upto 150000.
** Lift the MAX_COUNT to parse bigger array.
NOTE : Selecting the trend slowdown the timing efficiency.
@Author = ANKUR
Divyadrishti imaging lab, IIT Roorkee.
ankur89587@gmail.com; ankur@ph.iitr.ac.in
*/

#include "proj_dtrnd.h"




#define MAX_COUNT 150000

void proj_dtrnd(char* filename, int N_packets, double** data,int data_size, int trend,double* proj) {

    //filename= data file name containing data. 
    //data= input array.
    //data_size= length of array in data.
    int count = 0;
    double* amp = NULL; double* time = NULL;
    if (data == NULL)
    {
        // To read data from.dat file.
        int ret = 0;
        //char* filename = "outfile_vs.dat";
        FILE* fp;
        errno_t err = fopen_s(&fp, filename, "r");
        if (!fp)
        {
            printf("FILE NOT OPENED. \n");
            exit(0);
        }
        fseek(fp, 0L, SEEK_END);

        // calculating the size of the file
        long int len_fp = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        //printf("length of fp=%d \n ", len_fp);
        // Initialising the arrays to store time and amplitude data.
        amp = (double*)malloc(len_fp * sizeof(double));
        if (!amp)
        {
            printf(stderr, "Unable to allocate memory.");
            free(amp);
            //*x = '\0';
            return -1;
        }
        time = (double*)malloc(len_fp * sizeof(double));
        if (!time)
        {
            printf(stderr, "Unable to allocate memory.");
            free(time);
            //*time = '\0';
            return -1;
        }
        while (!feof(fp)               /* Check for the end of file*/
            && (count < MAX_COUNT)) /* To avoid memory corruption */
        {
            int ret = fscanf_s(fp, "%lf %lf", &time[count], &amp[count]);
            //printf(" Time = %e \t Data = %e \n", time[count], amp[count]);
            count++;
        }
        fclose(fp);
    }
    else
    {
        amp = (double*)malloc(data_size * sizeof(double));
        if (!amp)
        {
            printf(stderr, "Unable to allocate memory.");
            free(amp);
            //*x = '\0';
            return -1;
        }
        time = (double*)malloc(data_size * sizeof(double));
        if (!time)
        {
            printf(stderr, "Unable to allocate memory.");
            free(time);
            //*time = '\0';
            return -1;
        }
        count = data_size;
        for (int i = 0; i < count; i++)
        {
            time[i] = data[i][0];
            amp[i] = data[i][1];
        }       
    }

    int amp_size = count-1;                  // Last data point is random double value.
    //printf("Amp_size= %d \n", amp_size);
    // To remove any offset in amplitude.
    double amp_average = average(amp,amp_size);
    for (int i = 0; i < amp_size; i++)
    {
        amp[i] = amp[i] - amp_average;
    }
    // length of single wave packet.
    int len_packet = amp_size / N_packets;

    /*Some transmission data have noise only and does not contribute to
      the actual result.Discarding those signal files based on the amplitude difference.
      This code also checks if the data have trend.If significant trend found, signal is detrended.
      To check if the data have trend affecting the peak detection. */
    if (trend == 1) {
        int poly_degree = 20;
        double* poly_coeff = (double*)malloc(amp_size * sizeof(double));
        if (!poly_coeff)
        {
            printf(stderr, "Unable to allocate memory.");
            free(poly_coeff);
            return -1;
        }
        // polyfit return the coefficients(Pn) of PnX^(n-1) in [Pn Pn-1 Pn-2 ...].
        int polyfit_success = polyfit(amp_size, time, amp, poly_degree, poly_coeff);
        // polyval generate the polynomial value at Pn coeff. at given value of x.
        // defining array to store generated polynomial value.
        

        double* pn = (double*)malloc(amp_size * sizeof(double));
        if (!pn)
        {
            printf(stderr, "Unable to allocate memory.");
            free(pn);
            //*x = '\0';
            return -1;
        } 

        polyval(poly_coeff,poly_degree, time, amp_size,pn);

        // Detrending the signal amp.
        for (int j = 0; j < amp_size; j++)
        {
            amp[j] = amp[j] - pn[j];
        }
        free(pn);
        free(poly_coeff);
    }

    // Initialising arrays to store the peak data.
    // To store time peak data and locations.

    double* peak = NULL;
    peak = (double*)calloc(N_packets, sizeof(double));
    if (!peak)
    {
        printf(stderr, "Unable to allocate memory.");
        free(peak);
        return -1;
    }
    int* peak_locs = NULL;
    peak_locs = (int*)calloc(N_packets, sizeof(int));
    if (!peak_locs)
    {
        printf(stderr, "Unable to allocate memory.");
        free(peak_locs);
        return -1;
    }

    double* trough = NULL;
    trough = (double*)calloc(N_packets, sizeof(double));
    if (!trough)
    {
        printf(stderr, "Unable to allocate memory.");
        free(trough);
        return -1;
    }
    int* trough_locs = NULL;
    trough_locs = (int*)calloc(N_packets, sizeof(int));
    if (!trough_locs)
    {
        printf(stderr, "Unable to allocate memory.");
        free(trough_locs);
        return -1;
    }

    double* neg_amp = (double*)malloc(amp_size * sizeof(double));
    if (!neg_amp)
    {
        printf(stderr, "Unable to allocate memory.");
        free(neg_amp);
        return -1;
    }

    // Negative amplitude to find trough.
    for (int n = 0; n < amp_size; n++)
    {
        neg_amp[n] = -1.0 * amp[n];
    }

    // Finding peaks in each wave packet. 

    int dist = amp_size / N_packets;  // Size of one wave packet.
    //printf("The distance= %d \n", dist);
    findpeaks(amp, amp_size, dist, peak, peak_locs);
    findpeaks(neg_amp, amp_size, dist, trough, trough_locs);

    /*
    for (int i = 0; i < N_packets; i++)
    {
        //printf("plocs =%d \t nlocs=%d \n", plocs, nlocs);
        printf("peak= %e \t locs= %e \n", amp[peak_locs[i]], time[peak_locs[i]]);

        printf("npeak= %e \t nlocs= %e \n", amp[trough_locs[i]], time[trough_locs[i]]);
    }
    */
    /*
    for (int j = 0; j < amp_size; j++)
    {
        printf("ampl= %e \n", amp[j]);
    }
    */

    // Calculating the peak to peak value of each packet.
    // Initialising the array to store the ptp amplitudes.
    double* peaktopeak = (double*)malloc(N_packets * sizeof(double));
    if (!peaktopeak)
    {
        printf(stderr, "Unable to allocate memory.");
        free(peaktopeak);
        return -1;
    }

    for (int i = 0; i < N_packets; i++)
    {
        //printf("peak= %e \t trough= %e \n", peak[i], trough[i]); //Both value are returned positive.
        peaktopeak[i] = peak[i] + trough[i];
        //printf("Peaktopeak = %lf \n", peaktopeak[i]);
    }
    // Removing outliers, if exist.
    double* rm_ptp = (double*)malloc(N_packets * sizeof(double));
    if (!rm_ptp)
    {
        printf(stderr, "Unable to allocate memory.");
        free(rm_ptp);
        return -1;
    }
    int rm_size=0;
    rmoutliers(peaktopeak, N_packets, 50, rm_ptp, &rm_size);
    //printf("rm_size = %d  \n", rm_size);

    int rms_size = rm_size;
    *proj = average(rm_ptp, rms_size);

    // Free allocated memory.
    free(amp);
    free(neg_amp);
    free(time);
    
    free(peak);
    free(peak_locs);
    free(trough);
    free(trough_locs);
    free(peaktopeak);
    free(rm_ptp);
}


