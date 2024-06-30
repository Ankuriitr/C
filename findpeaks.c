/*This code finds the peaks and their location in the given dataset(array)
* peaks can be distinguished based on the local proximity(distance).
* It returns the detected peaks array and their location.
* 
* 
@Author = ANKUR
Divyadrishti imaging lab, IIT Roorkee.
ankur89587@gmail.com; ankur@ph.iitr.ac.in 
*/

#include "findpeaks.h"

// Maxx returns the maximum element in array with its location.
void maxx(double* arr, int size,double* maximum,int* location) {
    double max_value = arr[0];
    for (int i = 1; i < size; i++)
    {
        if (arr[i] > max_value)
        {
            max_value = arr[i];
            *location = i;
        }
        *maximum = max_value;
    }
}


void findpeaks(double* data, size_t size, int distance, double peaks[], int location[]) {
    // Number of elements in given distance.
    int N_elements = distance;
     int num_peaks= size / distance; // Num peaks to be found in the dataset.

    // Loop to find peaks.
    for (int p = 0; p < num_peaks; p++)
    {
        double* piece = (double*)malloc(N_elements * sizeof(double));
        if (!piece)
        {
            printf(stderr, "Unable to allocate memory.");
            free(piece);
            //*x = '\0';
            return -1;
        }
        int element = 0;
        for (int s = N_elements * p; s < N_elements * (p + 1); s++)
        {
            piece[element] = data[s];
            element++;
        }
        double peak = 0.0;
        int loc = 0.0;
        maxx(piece, element, &peak, &loc);
        peaks[p] = peak;
        location[p] = loc+(N_elements*p);
        free(piece);
    }

}