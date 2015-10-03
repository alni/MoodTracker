/*
Mood Tracker v1.2
https://github.com/alni/MoodTracker/
----------------------
The MIT License (MIT)

Copyright (c) 2015 Alexander Nilsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------
src/math_helpers.c
*/

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

int math_int_normalize(int value, int min, int max) {
  if (value < min) {
    value = max;
  } else if (value > max) {
    value = min;
  }
  return value;
}

