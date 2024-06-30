
#include "basicfn.h"

// To find the average of an array.
double average(double* array, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        sum = sum + array[i];
    }
    double average = sum / (double)size;
    return average;
}
// To find maximum in an array.
double maximum(double* arr, int size) {
    double max_value = arr[0];
    for (int i = 1; i < size; i++)
    {
        if (arr[i] > max_value)
        {
            max_value = arr[i];
        }
    }
    return max_value;
}
// To find the minimum in an array.
double minimum(double* arr, int size) {
    double min_value = arr[0];
    for (int i = 1; i < size; i++)
    {
        if (arr[i] < min_value)
        {
            min_value = arr[i];
            //printf(" i= %d  arr= %e   Min = %e \n", i, arr[i], min_value);
        }
    }
    return min_value;
}
// To find mean of only positive numbers or negative numbers in array.
double pnmean(double* array, int size, int type) {
    double sum = 0.0;
    for (int i = 1; i < size; i++)
    {
        if (type == 0)
        {
            if (array[i] > 0.0)
            {
                sum += array[i];
            }
        }
        else
        {
            if (array[i] < 0.0)
            {
                sum -= array[i];
            }
        }
    }
    sum = sum / size;
    return sum;
}

//rmoutliers remove outliers beyond the allowed percentage fluctuation(bound).
void rmoutliers(double* data, int size, int bound, double rmo_data[], int* rmo_data_size) {
    //bound= percentage to remove outliers.
    double avg = average(data, size);
    double bound_value = avg * ((double)bound / (double)100);
    double neg_bound = avg - bound_value;
    double pos_bound = avg + bound_value;
    //printf("In RMO avg=%lf  Bound_value=%lf  neg_bound=%lf  pos_bound=%lf \n",avg,bound_value,neg_bound,pos_bound);
    int count = 0;
    for (int n = 0; n < size; n++)
    {
        //printf("I m in for loop rmo \n");
        if (data[n]<pos_bound && data[n]>neg_bound)
        {
            //printf("I m in if loop rmo \n");
            rmo_data[count] = data[n];
            //printf("count=%d   rm_data = %lf  \n",count, rmo_data[count]);
            count++;
        }

    }
    *rmo_data_size = count;
}

// To find max in array from elements between array[start] and array[end]. 
double max_array_element(double* array, int start, int arr_end)
{
    double max_value = array[start];
    for (int i = start; i < arr_end; i++)
    {
        if (array[i] > max_value)
        {
            max_value = array[i];
        }
    }
    return max_value;
}

// Sleep function for delaying the process.
void waitFor(unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}
