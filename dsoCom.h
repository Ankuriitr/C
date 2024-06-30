// To control the digital wave generator.
// Author @ Ankur, DD imaging lab, IIT Roorkee.
#ifndef DSO_COM_H
#define DSO_COM_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <errno.h>
#include "visa.h"

void dsoOpen();
void dsoCom(const char* channel,char* output_file);
void dsoClose();

#endif
