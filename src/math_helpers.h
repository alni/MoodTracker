#pragma once
  
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#include "pebble.h"
  


int compare (const void * a, const void * b);
int math_int_get_median(int values[], int length);

int math_int_get_average(int values[], int length);