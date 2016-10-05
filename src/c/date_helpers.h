/*
Mood Tracker v1.2
https://github.com/alni/MoodTracker/
----------------------
The MIT License (MIT)

Copyright (c) 2015-2016 Alexander Nilsen

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
src/date_helpers.h
*/

#pragma once

/*
 * Function:  date_get_weekday 
 * ----------------------------------------------------------------------------
 *  Gets the current weekday.
 *  returns: current weekday from tm->tm_wday
 * ----------------------------------------------------------------------------
 */
int date_get_weekday();

/*
 * Function:  date_get_yearday 
 * ----------------------------------------------------------------------------
 *  Gets the current yearday.
 *  returns: current yearday from tm->tm_yday
 * ----------------------------------------------------------------------------
 */
int date_get_yearday();

/*
 * Function:  date_get_hour 
 * ----------------------------------------------------------------------------
 *  Gets the current hour.
 *  returns: current hour from tm->tm_hour
 * ----------------------------------------------------------------------------
 */
int date_get_hour();

