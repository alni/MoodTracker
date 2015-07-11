#include "pebble.h"
  
#include "overview_window.h"
#include "common.h"
#include "date_helpers.h"
#include "storage.h"

#define REPEAT_INTERVAL_MS 50

// You can define defaults for values in persistent storage
#define NUM_MOOD_DEFAULT 0

static Window *s_main_window;
 
static ActionBarLayer *s_action_bar;
static TextLayer *s_header_layer, *s_body_layer, *s_label_layer;
static GBitmap *s_icon_plus, *s_icon_minus;

static int s_num_mood = NUM_MOOD_DEFAULT;

static void update_text() {
  static char s_body_text[18];
  snprintf(s_body_text, sizeof(s_body_text), "%d Steps", s_num_mood);
  text_layer_set_text(s_body_layer, s_body_text);
}

static void increment_click_handler(ClickRecognizerRef recognizer, 
                                    void *context) {
  if (s_num_mood >= NUM_MOOD_MAX) {
    return;
  }
  s_num_mood++;
  update_text();
}

static void decrement_click_handler(ClickRecognizerRef recognizer, 
                                    void *context) {
  if (s_num_mood <= NUM_MOOD_MIN) {
    // Keep the counter at zero
    return;
  }

  s_num_mood--;
  update_text();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //persist_write_int(date_get_weekday(), s_num_mood);
  storage_save_mood(date_get_weekday(), s_num_mood);
  overview_window_push();
}


static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, 
                                          increment_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, 
                                          decrement_click_handler);
  
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_action_bar, window);
  action_bar_layer_set_click_config_provider(s_action_bar, 
                                             click_config_provider);

  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_UP, s_icon_plus);
  action_bar_layer_set_icon(s_action_bar, BUTTON_ID_DOWN, s_icon_minus);
#ifdef PBL_COLOR
  // Set the Window background color to a color that is similar to hospital 
  // walls
  window_set_background_color(window, GColorCadetBlue);
  // Set the Window background color to a color that is similar to medical staff 
  // uniforms
  action_bar_layer_set_background_color(s_action_bar, GColorCobaltBlue);
#endif

  int width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;

  s_header_layer = text_layer_create(GRect(4, 0, width, 60));
  text_layer_set_font(s_header_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(s_header_layer, GColorClear);
  text_layer_set_text(s_header_layer, "Mood Tracker");
  layer_add_child(window_layer, text_layer_get_layer(s_header_layer));

  s_body_layer = text_layer_create(GRect(4, 44, width, 60));
  text_layer_set_font(s_body_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(s_body_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_body_layer));

  s_label_layer = text_layer_create(GRect(4, 44 + 28, width, 60));
  text_layer_set_font(s_label_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text(s_label_layer, "of mood today");
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  update_text();
}

/*
 * Function: main_window_unload
 * ----------------------------------------------------------------------------
 *   Main window unload handler.
 *   Destroys the layers and the window.
 *
 *   window: the window that has been unloaded
 * ----------------------------------------------------------------------------
 */
static void main_window_unload(Window *window) {
  text_layer_destroy(s_header_layer);
  text_layer_destroy(s_body_layer);
  text_layer_destroy(s_label_layer);

  action_bar_layer_destroy(s_action_bar);
}

static void init() {
  s_icon_plus = gbitmap_create_with_resource(
    RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  s_icon_minus = gbitmap_create_with_resource(
    RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);

  // Get the mood from persistent storage for use if it exists, otherwise use 
  // the default
  s_num_mood = storage_read_mood(date_get_weekday());
    //persist_exists(date_get_weekday()) ? persist_read_int(date_get_weekday()) : NUM_MOOD_DEFAULT;

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "date_get_weekday() = %d", date_get_weekday());
  // Save the mood into persistent storage on app exit
  storage_save_mood(date_get_weekday(), s_num_mood);
  //persist_write_int(date_get_weekday(), s_num_mood);

  window_destroy(s_main_window);

  gbitmap_destroy(s_icon_plus);
  gbitmap_destroy(s_icon_minus);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
