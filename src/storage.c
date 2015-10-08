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
src/storage.c
*/

#include <pebble.h>
#include "storage.h"
#include "common.h"
  
static int s_moods[7];
  
/*void get_moods(int *output[], int length) {
  for (int i = 0; i < length; i++) {
    output[i] = read_mood(i);
  }
}*/
  
void storage_save_mood(int key, int mood) {
  switch(key) {
    case KEY_MOOD_SUN:
    case KEY_MOOD_MON:
    case KEY_MOOD_TUE:
    case KEY_MOOD_WED:
    case KEY_MOOD_THU:
    case KEY_MOOD_FRI:
    case KEY_MOOD_SAT:
      persist_write_int(key, mood);
      break;
  }
}

int storage_get_mood(int key) {
  int mood = s_moods[key];
  if (!mood) {
    mood = storage_read_mood(key);
    s_moods[key] = mood;
  }
  return mood;
}

int storage_read_mood(int key) {
  // Count number of launches
  int mood = 0;
  
#if APP_DEBUG
  mood = DUMMY_DATA[key];
#else
  // Check to see if a count already exists
  if (persist_exists(key)) {
    // Load stored count
    mood = persist_read_int(key);
  }
  #endif
  return mood;
}
  
void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // Update the TextLayer output
  static char s_count_buffer[32];
  snprintf(s_count_buffer, sizeof(s_count_buffer), "Count: %d", (int)new_tuple->value->int32);
  //text_layer_set_text(s_output_layer, s_count_buffer);
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Mood Min value:
  Tuple *mood_min_t = dict_find(iterator, KEY_MOOD_MIN);
  
  // Mood Max value:
  Tuple *mood_max_t = dict_find(iterator, KEY_MOOD_MAX);
  
  // Mood Step value:
  Tuple *mood_step_t = dict_find(iterator, KEY_MOOD_STEP);
  
  if (mood_min_t) {
    int mood_min = mood_min_t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mood Min = %d", mood_min);
    persist_write_int(KEY_MOOD_MIN, mood_min);
  }
  
  if (mood_max_t) {
    int mood_max = mood_max_t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mood Max = %d", mood_max);
    persist_write_int(KEY_MOOD_MAX, mood_max);
  }
  
  if (mood_step_t) {
    int mood_step = mood_step_t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mood Step = %d", mood_step);
    persist_write_int(KEY_MOOD_STEP, mood_step);
  }
}

/*void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
    // Long lived buffer
    static char s_buffer[64];

    // Process this pair's key
    switch (t->key) {
      case KEY_MOOD_SUN:
      case KEY_MOOD_MON:
      case KEY_MOOD_TUE:
      case KEY_MOOD_WED:
      case KEY_MOOD_THU:
      case KEY_MOOD_FRI:
      case KEY_MOOD_SAT:
        storage_save_mood(t->key, t->value->int32);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}*/

/*void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}*/

/*void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}*/

/*void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}*/

