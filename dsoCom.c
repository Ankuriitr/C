// To control the digital Oscilloscope.
// Author @ Ankur, DD imaging lab, IIT Roorkee.
#include "dsoCom.h"
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <visa.h>
*/

static ViSession defaultRM;
static ViSession instr;
static ViStatus status;
static ViUInt32 writeCount;
static ViUInt32 retCount;
static ViUInt32 time_div;
static ViUInt32 com_header;
static ViUInt32 query;

static unsigned char buffer[100];
static unsigned char t_buffer[100];
static unsigned char dt_buffer[100];
static unsigned char data_buffer[2000000];
static char stringinput[512];
static char datafile[512];


// To connnect to the DSO.
void dsoOpen() {
    viOpenDefaultRM(&defaultRM); // Open the default Visa instr.

    status = viOpen(defaultRM, "USB0::0x05FF::0x1023::3517N53447::INSTR", VI_NULL, VI_NULL, &instr);
    if (status < VI_SUCCESS)
    {
        printf("Cannot open a session to the device.\n");
        //continue;
    }
    else
    {
        printf("DSO is connected. \n");
    }
}

// To acquire data from the DSO.
void dsoCom(const char* channel,char* output_file)
{
    char* data;
    double* file_data;
    double str_doub;
    double* time_array;
    int  i, j;
    size_t len_data, d_array_len;
    int len_Ampdata;
    int count = 2000000;   // Data counts. Counts in the data acquired.
    char command_H_Interval[100];
    char command_Inspect_Arr[100];

    strcpy_s(stringinput, 512, "*IDN?\n");

    viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
    viRead(instr, buffer, 100, &retCount);
    //printf("\nDevice %*s\n", retCount, buffer);


    // To set the communication header off.
    viWrite(instr, (ViBuf)"COMM_HEADER OFF", (ViUInt32)strlen("COMM_HEADER OFF"), VI_NULL);

    viWrite(instr, (ViBuf)"COMM_HEADER?", (ViUInt32)strlen("COMM_HEADER?"), VI_NULL);
    viRead(instr, buffer, 100, &com_header);
    //printf("\nComm Header:  %*s\n", com_header, buffer);


    viWrite(instr, (ViBuf)"TIME_DIV?", (ViUInt32)strlen("TIME_DIV?"), VI_NULL);
    viRead(instr, t_buffer, 100, &time_div);
    //printf("\nTime Division:  %*s\n", time_div, t_buffer);

    /** Evaluation of the dt of time array */
    //printf("Time Division: %s", t_buffer);
    char* unconverted;
    double tdiv = strtod(t_buffer, &unconverted);
    //printf("TDIV = %e \n", tdiv);
    double total_time = tdiv * 10.0; // Display have 10 time divisions each TDIV=tdiv .

    sprintf_s(command_H_Interval, 100, "%s:INSPECT? HORIZ_INTERVAL", channel);
    //printf("Command 1 = %s \n", command_H_Intervel);
    //strcat(command_H_Interval, channel);
    sprintf_s(command_Inspect_Arr, 100, "%s:INSPECT? DATA_ARRAY_1", channel);
    //printf("Command 2 = %s \n", command_Inspect_Arr);
    //strcat(command_Inspect_Arr, channel);


    viWrite(instr, (ViBuf)command_H_Interval, (ViUInt32)strlen(command_H_Interval), VI_NULL);
    viRead(instr, dt_buffer, 100, &time_div);
    //printf("\nTime Division dt :  %s \n", dt_buffer);

    char ptr[20];
    for (j = 0; j <= 12; j++)
    {
        ptr[j] = dt_buffer[22 + j];
    }
    //printf(" ptr= %s \n", ptr);
    char* uncon;
    double dtdiv = strtod(ptr, &uncon);
    //printf("dt Time DIV = %e \n", dtdiv);
    /** Evaluation of dt complete.*/

    // To get the waveform data in string array form.
    viWrite(instr, (ViBuf)command_Inspect_Arr, (ViUInt32)strlen(command_Inspect_Arr), VI_NULL);

    //viReadToFile(instr, datafile, count, VI_NULL);           // Save the waveform data directly into .dat file.

    viBufRead(instr, data_buffer, count, &retCount);

/**To convert the read buffer data into array for further processing.*/
    len_data = strlen(data_buffer);
    data = (char*)malloc(sizeof(char) * len_data);  // Initialising array to store data points.
    //printf("Earlier length= %zu \n", len_data);
    // To convert the string into array.
    j = 0;
    for (i = 0; i < len_data; i++)
    { //printf("I m in for loop. \n");
        if (data_buffer[i] != '"' || data_buffer[i] == '\r' || data_buffer[i] == '\n')   // Replacing some characters from the data.
        {
            data[j] = data_buffer[i];
            //c=data_buffer[j];
            j++;
            //printf("I m in IF loop. \n");
        }
    }
    //printf(" %s \n",data);
    len_data = strlen(data);
    //printf("New length= %zu", len_data);
    d_array_len = len_data / 14;           // One data points consist of 14 character.
    //printf(" # data points= %zu \n", d_array_len);

    file_data = (double*)malloc(sizeof(double) * d_array_len);  // Initialising array to store data points.
    time_array = (double*)malloc(sizeof(double) * d_array_len);  // Initialising array to store data points.
    if (!time_array)
    {
        printf("Unable to allocate memory.");
    }
    if (!file_data)
    {
        printf("Unable to allocate memory.");
    }
    /** To Save each data point into array index
    /* get the first token */
    //char* output_file = "outfile_vs.dat";
    FILE* fp=NULL;
    errno_t error1 = fopen_s(&fp,output_file, "w");   // Final array data will be saved in this file.
    if (!fp)
    {
        printf(stderr,"Error: can't open file to write.\n");
        exit(0);
    }

    char* delim = "  "; // input separated by spaces
    char* token = NULL;

    token = strtok(data, delim);
    /* walk through other tokens */
    i = 0;
    while (token != NULL) {
        char* unconvtd;
        str_doub = strtod(token, &unconvtd);
        token = strtok(NULL, delim);
        if (i >= 1)
        {
            time_array[i - 1] = dtdiv * (i - 1);
            fprintf(fp, "%e\t%e\n", time_array[i - 1], str_doub);       // Writing array elements to data file.
            file_data[i - 1] = str_doub;
            //printf("%e \n",file_data[i-1]);
        }
        i++;
        //printf("%s \n",token);
        //printf( " %s\n", token );
    }
    fclose(fp);
    len_Ampdata = i - 1;
    //printf(" Size of File Data Array= %d \n", len_Ampdata);
    /** END : Data from string converted to array */

    free(data);
    free(time_array);
    free(file_data);
    
}

// To Close the DSO Connection
void dsoClose() {
    viClose(defaultRM);
    printf(" DSO connection is closed. \n");
}