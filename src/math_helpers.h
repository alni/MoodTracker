#pragma once
  
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#include "pebble.h"
  


int math_int_compare (const void * a, const void * b);

/*
 * Function: math_int_get_median
 * ----------------------------------------------------------------------------
 *   Calculates the median of int values in a list.
 *
 *   values: array of int values
 *   length: the length of the values array
 *
 *   returns: the median of values
 * ----------------------------------------------------------------------------
 */
int math_int_get_median(int values[], int length);

/*
 * Function: math_int_get_average
 * ----------------------------------------------------------------------------
 *   Calculates the average of int values in a list.
 *
 *   values: array of int values
 *   length: the length of the values array
 *
 *   returns: the average of values
 * ----------------------------------------------------------------------------
 */
int math_int_get_average(int values[], int length);

/*
 * Function:  math_int_find_bezier_control_point 
 * ----------------------------------------------------------------------------
 * calculates a bezier curve control point using:
 *    ((p * 100) - (100 - t)) / t
 *
 *  p: the point along the line
 *  t: the percentage along the line the point lies
 *
 *  returns: the control point
 * ----------------------------------------------------------------------------
 */
int math_int_find_bezier_control_point(int p, int t);

int math_int_normalize(int value, int min, int max);