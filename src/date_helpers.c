#include <pebble.h>
#include "date_helpers.h"
  
int date_get_weekday() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  return t->tm_wday;
}

int date_get_yearday() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  return t->tm_yday;
}

int date_get_hour() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  return t->tm_hour;
}
