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
src/storage.c
*/

#include <pebble.h>
#include "storage.h"
#include "common.h"
  
static int s_moods[7];
static int s_mood_backup = -1;

#define NUM_ITEMS 7;

static int s_index = 0;

int storage_read_int(int key, int def) {
  // Count number of launches
  int val = 0;

  // Check to see if a key already exists
  if (persist_exists(key)) {
    // Load stored int
    val = persist_read_int(key);
  } else {
    val = def;
  }
  return val;
}

int storage_get_mood_backup_flag() {
  int val = 0;
  // Check to see if a key already exists
  if (persist_exists(KEY_MOOD_BACKUP)) {
    // Load stored int
    val = persist_read_int(KEY_MOOD_BACKUP);
  } else {
    val = 0; // Defaults to 0 (off)
  }
  
  s_mood_backup = val;
  return val;
}
  
/*void get_moods(int *output[], int length) {
  for (int i = 0; i < length; i++) {
    output[i] = read_mood(i);
  }
}*/
  
void storage_save_mood(int key, int mood, bool only_diff) {
  switch(key) {
    case KEY_MOOD_SUN:
    case KEY_MOOD_MON:
    case KEY_MOOD_TUE:
    case KEY_MOOD_WED:
    case KEY_MOOD_THU:
    case KEY_MOOD_FRI:
    case KEY_MOOD_SAT:
      if (!only_diff || (storage_get_mood(key) != mood && mood != 0)) {
        // Only save the mood if "only_diff" is false (always replace), OR if
        // the already stored mood is different from "mood" AND "mood" is not 0
        s_moods[key] = mood;
        persist_write_int(key, mood);
      }
      break;
  }
  s_mood_backup = storage_get_mood_backup_flag();
  APP_LOG(APP_DEBUG, "Backup Flag: %d", s_mood_backup);
  if (s_mood_backup == 1) { // Only back-up if the user wishes so
    // Declare the dictionary's iterator
    DictionaryIterator *out_iter;
    
    // Prepare the outbox buffer for this message
    AppMessageResult result = app_message_outbox_begin(&out_iter);
    
    if (result == APP_MSG_OK) {    
      // Add the current mood data
      dict_write_int(out_iter, s_index, &s_moods[s_index], sizeof(int), true);
      
      // Send the message
      result = app_message_outbox_send();
      
      // Check the result
      if (result != APP_MSG_OK) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
      }
    } else {
      // The outbox cannot be used right now
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
    }
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
  
  // Mood Backup value:
  Tuple *mood_backup_t = dict_find(iterator, KEY_MOOD_BACKUP);
  
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
  
  if (mood_backup_t) {
    int mood_backup = mood_backup_t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mood Backup = %d", mood_backup);
    persist_write_int(KEY_MOOD_BACKUP, mood_backup);
    
    s_mood_backup = mood_backup + 0;
  }
  
  if (s_mood_backup == 1) { // Only restore if the user wishes so
    inbox_received_read_moods(iterator, context);
  }
}

void inbox_received_read_moods(DictionaryIterator *iterator, void *context) {
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
        storage_save_mood(t->key, t->value->int32, true);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

/*void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}*/

/*void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}*/

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
  
  if (s_mood_backup == 1) { // Only back-up if the user wishes so
    // Increment the index
    s_index++;
  
    if (s_index < 7) {
      // Send the next item
      DictionaryIterator *iter;
      if(app_message_outbox_begin(&iter) == APP_MSG_OK) {
        dict_write_int(iter, s_index, &s_moods[s_index], sizeof(int), true);
        app_message_outbox_send();
      }
    } else {
      // We have reached the end of the sequence
      s_index = 0;
      APP_LOG(APP_LOG_LEVEL_INFO, "All transmission complete!");
    }
  }
}

