#pragma once
  
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#include "pebble.h"
  


int math_int_compare (const void * a, const void * b);
int math_int_get_median(int values[], int length);

int math_int_get_average(int values[], int length);

int math_int_find_bezier_control_point(int p, int t);

int math_int_find_graph_point(int value, int min, int max, GRect bounds);