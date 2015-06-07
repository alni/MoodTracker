#include <pebble.h>
#include "math_helpers.h"
  
int math_int_compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
  
int math_int_get_median(int values[], int length) {
  qsort (values, length, sizeof(int), math_int_compare);
  return values[length/2];
}

int math_int_get_average(int values[], int length) {
  int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += values[i];
  }
  return sum / length;
}

int math_int_find_bezier_control_point(int p, int t) {
  return (((p * 100) - (100 - t)) / t);
}