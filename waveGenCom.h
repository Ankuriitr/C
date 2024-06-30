// To control the digital wave generator.
// Author @ Ankur, DD imaging lab, IIT Roorkee.
#ifndef WAVE_GEN_COM_H
#define WAVE_GEN_COM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <visa.h>

void wavCom(int frequency, int amplitude);
//char* getData();
void writeWData(char* filename);

#endif

