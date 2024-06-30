// To control the digital wave generator.
// Author @ Ankur, DD imaging lab, IIT Roorkee.

#include "waveGenCom.h"

static ViSession defaultRM1;
static ViSession instr1;
static ViStatus status1;
static ViUInt32 writeCount1;
static ViUInt32 retCount1;

static unsigned char buffer[256];

void wavCom(int frequency, int amplitude)
{
    char wavetype[] = "PULSE"; char z[200];
    char freq[10]; char amp[10];
    printf("Input Frequency= %d and Amplitude= %d", frequency, amplitude);

    sprintf_s(freq, 10, "%d", frequency);
    sprintf_s(amp, 10, "%d", amplitude);

    char rise_time[] = "0.0000000104";
    char fall_time[] = "0.0000000084";
    char pulse_width[] = "0.0000000290";

    viOpenDefaultRM(&defaultRM1); // Open the default Visa instr.

    status1 = viOpen(defaultRM1, "USB0::0xF4EC::0xEE38::SDG2XCAD2R3765::INSTR", VI_NULL, VI_NULL, &instr1);
    if (status1 < VI_SUCCESS)
    {
        printf("Cannot open a session to the device.\n");
        return 0;
    }
    printf(" \n Wave Generator Connected  \n");
    sprintf_s(z, 200, "C1:BSWV WVTP,%s,FRQ,%s,AMP,%s,WIDTH,%s,RISE,%s,FALL,%s \n", wavetype, freq, amp, pulse_width, rise_time, fall_time);
    printf("%s \n", z);
    viWrite(instr1, (ViBuf)z, (ViUInt32)strlen(z), VI_NULL);
    viWrite(instr1, (ViBuf)"C1:OUTP ON", (ViUInt32)strlen("C1:OUTP ON"), VI_NULL);
    viWrite(instr1, (ViBuf)"C1:BSWV?", (ViUInt32)strlen("C1:BSWV?"), VI_NULL);
    status1 = viRead(instr1, buffer, 256, &retCount1);

    if (status1 < VI_SUCCESS)
    {
        printf("Error reading a response from the device.");
    }

    viClose(defaultRM1);
}
void writeWData(char* filename)
{
    FILE* fp = NULL;
    errno_t error=fopen_s(&fp,filename, "w");
    if (fp == NULL) {
        printf(stderr,"\n The file could not be opened");
        exit(1);
    }
    fprintf(fp, "Wave Data =%*s \n", retCount1, buffer);
    fclose(fp);
}
/*  NOT Working.
char* getData()
{
    char out[200];
    sprintf_s(out, 200, "Wave Data= %*s", retCount, buffer);
    return out;
}
*/

