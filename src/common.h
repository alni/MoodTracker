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
src/common.h
*/

#pragma once

#define APP_DEBUG false
#define DEBUG_DAY 3
  
#define NUM_MOOD_MIN 0
#define NUM_MOOD_MAX 10
#define NUM_MOOD_STEPS abs(NUM_MOOD_MIN) + abs(NUM_MOOD_MAX)
  
#if APP_DEBUG
static int DUMMY_DATA[] = {
  //-3, 2, 5, 0, 10, 1, -10
  4, 6, 8, 5, 10, 6, 0
  //7, 8, 7, 7, 5, 6, 5
};
#endif
