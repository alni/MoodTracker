#include "overview_window.h"
#include "common.h"
#include "storage.h"
#include "math_helpers.h"
#include "gpath_builder.h"
  
#define MAX_POINTS 512

static Window *s_main_window;
static TextLayer *s_text_layer;

static Layer *s_canvas_layer;

GFont s_res_bitham_42_medium_numbers;
GFont s_res_bitham_42_light;
GFont s_res_font_days ;

static GPath *s_path;


void set_mood() {
  int moods[7];
  for (int i = 0; i < 7; i++) {
    moods[i] = read_mood(i);
  }
  int avg = math_int_get_average(moods, 7);
  int median = math_int_get_median(moods, 7);
  static char s_buffer[128];
  // Compose string of all data for 3 samples
  snprintf(s_buffer, sizeof(s_buffer), "a: %d | m: %d", avg, median);
  text_layer_set_text(s_text_layer, s_buffer);
}

void draw_days(GRect bounds, GContext *ctx) {
  
}

void draw_graph_part_day(GContext *ctx, GRect bounds, char* s_day, int _day1) {
  graphics_draw_text(ctx, s_day, s_res_font_days, GRect(2+(20*_day1), (bounds.size.h/2)-21, 24, 21), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}



void draw_graph_part_base(GContext *ctx, GRect bounds, char* s_day, int _day0, int _day1, GPoint center, int hspacing, GPathBuilder *builder, bool first_time) {
  int _mood0 = read_mood(_day0);
  GPoint p0 = GPoint(12+2+(20*_day0), center.y-(_mood0*hspacing));
  int _mood1 = read_mood(_day1);
  GPoint p1 = GPoint(12+2+(20*_day1), center.y-(_mood1*hspacing));
  graphics_context_set_stroke_color(ctx, GColorWhite);
#ifdef PBL_PLATFORM_APLITE 
  graphics_draw_line(ctx, p0, p1);
#elif PBL_PLATFORM_BASALT
  int c_x = (p0.x + p1.x) / 2;
  int c_y = (p0.y + p1.y) / 2;
  int a_x = (p0.x + c_x) / 2;
  int a_y = math_int_find_bezier_control_point(c_y, 100); //(((c_y * 100) - (100 - 100)) / 100) / 1;
    //(p0.y + c_y) / 2;
  int b_x = (p1.x + c_x) / 2;
  int b_y = math_int_find_bezier_control_point(c_y, 100); //(((c_y * 100) - (100 - 100)) / 100) / 1;
    //(p1.y + c_y) / 2;
  GPoint p_a = GPoint(a_x, a_y);
  GPoint p_b = GPoint(b_x, b_y);
  if (first_time == true) {
    gpath_builder_move_to_point(builder, p0);
  }
  gpath_builder_curve_to_point(builder, p1, p_b, p_a);
#endif
  draw_graph_part_day(ctx, bounds, s_day, _day0);
}

void draw_graph_part_start(GContext *ctx, GRect bounds, char* s_day, int _day0, int _day1, GPoint center, int hspacing, GPathBuilder *builder) {
  draw_graph_part_base(ctx, bounds, s_day, _day0, _day1, center, hspacing, builder, true);
}

void draw_graph_part_parts(GContext *ctx, GRect bounds, char* s_day, int _day0, int _day1, GPoint center, int hspacing, GPathBuilder *builder) {
  draw_graph_part_base(ctx, bounds, s_day, _day0, _day1, center, hspacing, builder, false);
}

void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  if (s_path) {
    gpath_destroy(s_path);
  }
  GRect bounds = layer_get_bounds(this_layer);
  GRect _bounds = GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h / 2);
  
  
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);

  
  // Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  // Get the center of the screen (non full-screen)
  GPoint center = GPoint((bounds.size.w / 2) - bounds.origin.x, (bounds.size.h / 2) - bounds.origin.y);
  int hspacing = bounds.size.h / 20;
  
  // Draw the mood lines
  graphics_context_set_fill_color(ctx, GColorWhite);
  

#ifdef PBL_PLATFORM_APLITE
  int middle_h = hspacing*2;
  int other_h = (bounds.size.h/2) - middle_h;
  int middle_y = other_h + (middle_h / 2);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  //graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(-1, middle_y, bounds.size.w+2, middle_h));
#elif PBL_PLATFORM_BASALT
  int middle_h = hspacing*2;
  int other_h = (_bounds.size.h/2) - middle_h;
  int middle_y = other_h + (middle_h / 2);
  graphics_context_set_fill_color(ctx, GColorMidnightGreen);
  graphics_fill_rect(ctx, GRect(0, 0, _bounds.size.w, other_h+hspacing), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWindsorTan);
  graphics_fill_rect(ctx, GRect(0, middle_y, _bounds.size.w, middle_h), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBulgarianRose);
  graphics_fill_rect(ctx, GRect(0, middle_h+middle_y, _bounds.size.w, other_h+hspacing), 0, GCornerNone);
#endif
  
  graphics_context_set_stroke_color(ctx, GColorBlack);
#if PBL_PLATFORM_BASALT
  graphics_context_set_stroke_width(ctx, 1);
#endif
  int i = NUM_MOOD_MIN;
  /*for (; i <= NUM_MOOD_MAX; i++) {
    GPoint p0 = GPoint(0, center.y-(i*hspacing));
    GPoint p1 = GPoint(bounds.size.w, center.y-(i*hspacing));
    graphics_draw_line(ctx, p0, p1);
  }*/
  
  // Draw the 'base line'
  /*GPoint p0 = GPoint(0, center.y);
  GPoint p1 = GPoint(144, center.y);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, p0, p1);*/
#if PBL_PLATFORM_BASALT
  graphics_context_set_stroke_width(ctx, 2);
#endif
  draw_graph_part_start(ctx, bounds, "su", KEY_MOOD_SUN, KEY_MOOD_MON, center, hspacing, builder);
  draw_graph_part_parts(ctx, bounds, "mo", KEY_MOOD_MON, KEY_MOOD_TUE, center, hspacing, builder);
  draw_graph_part_parts(ctx, bounds, "tu", KEY_MOOD_TUE, KEY_MOOD_WED, center, hspacing, builder);
  draw_graph_part_parts(ctx, bounds, "we", KEY_MOOD_WED, KEY_MOOD_THU, center, hspacing, builder);
  draw_graph_part_parts(ctx, bounds, "th", KEY_MOOD_THU, KEY_MOOD_FRI, center, hspacing, builder);
  draw_graph_part_parts(ctx, bounds, "fr", KEY_MOOD_FRI, KEY_MOOD_SAT, center, hspacing, builder);  draw_graph_part_day(ctx, bounds, "sa", KEY_MOOD_SAT);
  
#ifdef PBL_PLATFORM_BASALT
  s_path = gpath_builder_create_path(builder);
#endif
  gpath_builder_destroy(builder);
  
  //gpath_move_to(s_path, GPoint((int16_t)(bounds.size.w/2), (int16_t)(bounds.size.h/2)));
#ifdef PBL_PLATFORM_APLITE
  //gpath_draw_outline(ctx, s_path);
#elif PBL_PLATFORM_BASALT
  gpath_draw_outline_open(ctx, s_path);
#endif
  
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 42));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, (Layer *)s_text_layer);
  set_mood();

  // Create Layer
  s_canvas_layer = layer_create(GRect(0, 42, bounds.size.w, (bounds.size.h-42)*2));
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
    //s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
    s_res_bitham_42_light = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
    s_res_font_days = fonts_get_system_font(FONT_KEY_GOTHIC_18  );
    
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}