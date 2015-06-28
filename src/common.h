#pragma once

#define APP_DEBUG false
  
#define NUM_MOOD_MIN 0
#define NUM_MOOD_MAX 10
#define NUM_MOOD_STEPS abs(NUM_MOOD_MIN) + abs(NUM_MOOD_MAX)
  
#if APP_DEBUG
static int DUMMY_DATA[] = {
  //-3, 2, 5, 0, 10, 1, -10
  //4, 6, 8, 5, 10, 6, 0
  7, 8, 7, 7, 5, 6, 5
};
#endif
