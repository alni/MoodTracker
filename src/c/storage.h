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
src/storage.h
*/

#pragma once

#define KEY_MOOD_SUN 0
#define KEY_MOOD_MON 1
#define KEY_MOOD_TUE 2
#define KEY_MOOD_WED 3
#define KEY_MOOD_THU 4
#define KEY_MOOD_FRI 5
#define KEY_MOOD_SAT 6
  
#define KEY_WEEKSTART 512
  
#define KEY_MOOD_MIN 1001
#define KEY_MOOD_MAX 1002
#define KEY_MOOD_STEP 1003
#define KEY_MOOD_BACKUP 1004
#define KEY_REMINDER_DAYS 2001
#define KEY_MOODS = 3001

#include "pebble.h"

int  storage_read_int(int key, int def);
  
void storage_get_moods(int *output[], int length);

int storage_get_mood(int key);

int storage_get_mood_backup_flag();

/*
 * Function: storage_save_mood
 * ----------------------------------------------------------------------------
 *   Saves mood for day to persistent storage.
 *   If backup is enabled, also sends the mood for day to be stored on the
 *   phone.
 *
 *   key: the key of the day for the mood to save. One of:
 *        KEY_MOOD_SUN, KEY_MOOD_MON, KEY_MOOD_TUE, KEY_MOOD_WED,
 *        KEY_MOOD_THU, KEY_MOOD_FRI, KEY_MOOD_SAT
 *   val: the mood to save to persistent storage for key
 *   only_diff: whether the mood should only be saved if it is different than
 *              the already stored mood
 *
 * ----------------------------------------------------------------------------
 */
void storage_save_mood(int key, int val, bool only_diff);

/*
 * Function: storage_get_mood
 * ----------------------------------------------------------------------------
 *   Reads mood for day from persistent storage.
 *
 *   key: the key of the day for the mood to lookup. One of:
 *        KEY_MOOD_SUN, KEY_MOOD_MON, KEY_MOOD_TUE, KEY_MOOD_WED,
 *        KEY_MOOD_THU, KEY_MOOD_FRI, KEY_MOOD_SAT
 *
 *   returns: the found mood for key or 0 if not found
 * ----------------------------------------------------------------------------
 */
int  storage_read_mood(int key);
  
void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);

/*
 * Function: inbox_received_read_moods
 * ----------------------------------------------------------------------------
 *   Reads moods from app message inbox and stores them in the persistent 
 *   storage. 
 *   Called by inbox_received_callback().
 *   
 *   iterator: the DictionaryIterator to read moods from key of days. Checks 
 *   for the following keys:
 *        KEY_MOOD_SUN, KEY_MOOD_MON, KEY_MOOD_TUE, KEY_MOOD_WED,
 *        KEY_MOOD_THU, KEY_MOOD_FRI, KEY_MOOD_SAT
 * ----------------------------------------------------------------------------
 */
void inbox_received_read_moods(DictionaryIterator *iterator, void *context);
/*
 * Function: inbox_received_callback
 * ----------------------------------------------------------------------------
 *   Called after app message has been received in the message inbox.
 *   
 *   Reads user settings from app message inbox and stores them in the 
 *   persistent storage. 
 *   Also calls inbox_received_read_moods().
 *   
 *   iterator: the DictionaryIterator to user settings from. Checks for the 
 *   following keys:
 *        KEY_MOOD_MIN, KEY_MOOD_MAX, KEY_MOOD_STEP, KEY_MOOD_BACKUP
 * ----------------------------------------------------------------------------
 */
void inbox_received_callback(DictionaryIterator *iterator, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
/*
 * Function: outbox_sent_callback
 * ----------------------------------------------------------------------------
 *   Called after app message has been sent to the message outbox.
 *   
 *   If backup is enabled, sends moods to app message outbox and stores them on
 *   the phone.
 *   Sends one mood at a time (starting with KEY_MOOD_MON, 1) until the last 
 *   mood has been sent (KEY_MOOD_SAT, 6). It is assumed that the first mood 
 *   (KEY_MOOD_SUN, 0) has already been sent to app message outbox.
 * ----------------------------------------------------------------------------
 */
void outbox_sent_callback(DictionaryIterator *iterator, void *context);
