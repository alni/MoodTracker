#include "overview_window.h"
#include "storage.h"

static Window *s_main_window;
static Layer *s_canvas_layer;

void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);

  // Get the center of the screen (non full-screen)
  //GPoint center = GPoint(bounds.size.w / 2, (bounds.size.h / 2));
  GPoint center = GPoint((bounds.size.w / 2) - bounds.origin.x, (bounds.size.h / 2) - bounds.origin.y);
  int hspacing = bounds.size.h / 20;

  // Draw the 'loop' of the 'P'
  /*graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 40);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 35);*/ 
  
  // Draw the mood lines
  graphics_context_set_fill_color(ctx, GColorBlack);
  
  int _day = KEY_MOOD_SUN;
  int _mood = read_mood(_day);
  GPoint p0 = GPoint(2, center.y-(_mood*hspacing));
  //GPoint p0 = GPoint(2, center.y-(_mood*8));
  _day = KEY_MOOD_MON;
  _mood = read_mood(_day);
  GPoint p1 = GPoint(2+20, center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+20, center.y-(_mood*hspacing));
  _day = KEY_MOOD_TUE;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*2), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);  
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+(20*2), center.y-(_mood*hspacing));
  _day = KEY_MOOD_WED;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*3), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+(20*3), center.y-(_mood*hspacing));
  _day = KEY_MOOD_THU;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*4), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+(20*4), center.y-(_mood*hspacing));
  _day = KEY_MOOD_FRI;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*5), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+(20*5), center.y-(_mood*hspacing));
  _day = KEY_MOOD_SAT;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*6), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
  
  p0 = GPoint(2+(20*6), center.y-(_mood*hspacing));
  _day = KEY_MOOD_SUN;
  _mood = read_mood(_day);
  p1 = GPoint(2+(20*7), center.y-(_mood*hspacing));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);

  // Draw the 'base line'
  p0 = GPoint(0, center.y);
  p1 = GPoint(144, center.y);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create Layer
  s_canvas_layer = layer_create(GRect(0, 42, bounds.size.w, bounds.size.h-42));
  layer_add_child(window_layer, s_canvas_layer);
  
  // Set the update_proc
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  //layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  //menu_layer_destroy(s_menu_layer);
  //text_layer_destroy(s_list_message_layer);
  layer_destroy(s_canvas_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void overview_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}