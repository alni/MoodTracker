#include <pebble.h>
#include "storage.h"
#include "common.h"
  
  
/*void get_moods(int *output[], int length) {
  for (int i = 0; i < length; i++) {
    output[i] = read_mood(i);
  }
}*/
  
void save_mood(int key, int mood) {
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



int read_mood(int key) {
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
        save_mood(t->key, t->value->int32);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

