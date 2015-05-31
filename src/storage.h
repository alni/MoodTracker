#pragma once

#define KEY_MOOD_SUN 0
#define KEY_MOOD_MON 1
#define KEY_MOOD_TUE 2
#define KEY_MOOD_WED 3
#define KEY_MOOD_THU 4
#define KEY_MOOD_FRI 5
#define KEY_MOOD_SAT 6

#include "pebble.h"
  
  
void get_moods(int *output[], int length);
  
void save_mood(int key, int val);
int  read_mood(int key);
  
void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);

void inbox_received_callback(DictionaryIterator *iterator, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);