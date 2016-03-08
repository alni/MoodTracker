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
src/overview_window.c
*/

#include "overview_window.h"
#include "common.h"
#include "storage.h"
#include "math_helpers.h"
#include "date_helpers.h"
#include "gpath_builder.h"

#define TOP_TEXT_H 42
#define MAX_POINTS 512
  

#define GRAPH_CURVE_HEIGHT 105
#define GRAPH_POINT_SPACE 10
#define GRAPH_AREA_GOOD_Y 0
#define GRAPH_AREA_GOOD_H 39
#define GRAPH_AREA_NEUTRAL_Y 39
#define GRAPH_AREA_NEUTRAL_H 27
#define GRAPH_AREA_BAD_Y 66
#define GRAPH_AREA_BAD_H 39
#define GRAPH_AREA_LABELS_H 21

  
static Window *s_main_window;
static TextLayer *s_text_layer;

static Layer *s_canvas_layer;

GFont s_res_bitham_42_light;
GFont s_res_font_days ;

static GPath *s_path;
static int s_mood_min = NUM_MOOD_MIN;
static int s_mood_max = NUM_MOOD_MAX;
static int s_mood_step = NUM_MOOD_STEP;
static int s_num_moods = NUM_MOOD_MAX;
static int s_moods[] = {0,0,0,0,0,0,0};

static int s_mood_points[] = {105, 95, 84, 74, 63, 53, 42, 32, 21, 11, 0};
static int s_day_points[] = {14, 34, 54, 74, 94, 124, 134};
static int s_label_points[] = {2, 22, 42, 62, 82, 102, 122};
static char* s_label_strings[] = {"su", "mo", "tu", "we", "th", "fr", "sa"};

static char* get_label_string(int day) {
  if (day > 6) {
    day -= 7;
  }
  return s_label_strings[day];
}

void set_mood() {
  //int moods[7];
  for (int i = 0; i < 7; i++) {
    s_moods[i] = storage_read_mood(i);
  }
  int avg = math_int_get_average(s_moods, 7);
  int median = math_int_get_median(s_moods, 7);
  static char s_buffer[128];
  // Compose string of all data for average and median
  snprintf(s_buffer, sizeof(s_buffer), "a: %d | m: %d", avg, median); 
  text_layer_set_text(s_text_layer, s_buffer);
} 

/*
 * Function: draw_mood_areas
 * ----------------------------------------------------------------------------
 *   Draws mood rectangle aresd
 *
 *   ctx: the drawing context
 *   bounds_size_w: the drawing bounds width
 * ----------------------------------------------------------------------------
 */
void draw_mood_areas(GContext *ctx, int bounds_size_w) {
#ifdef PBL_BW
  // On monochrome screens, only draw the middle (OK (4-6) mood) rectangle
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  graphics_draw_rect(ctx, GRect(-1, GRAPH_AREA_NEUTRAL_Y, bounds_size_w+2, 
                                GRAPH_AREA_NEUTRAL_H));
#elif PBL_COLOR
  // On color screens, draw the Good, OK and Bad mood rectangles
  
  // Set the top (Good (7-10) mood) rectangle to a positive color
  graphics_context_set_fill_color(ctx, GColorMidnightGreen);
  graphics_fill_rect(ctx, GRect(0, GRAPH_AREA_GOOD_Y, bounds_size_w, 
                                GRAPH_AREA_GOOD_H), 0, GCornerNone);
  
  // Set the middle (OK (4-6) mood) rectangle to a neutral color
  graphics_context_set_fill_color(ctx, GColorWindsorTan);
  graphics_fill_rect(ctx, GRect(0, GRAPH_AREA_NEUTRAL_Y, bounds_size_w, 
                                GRAPH_AREA_NEUTRAL_H), 0, GCornerNone);
  
  // Set the bottom (Bad (0-3) mood) rectangle to a negative color
  graphics_context_set_fill_color(ctx, GColorBulgarianRose);
  graphics_fill_rect(ctx, GRect(0, GRAPH_AREA_BAD_Y, bounds_size_w, 
                                GRAPH_AREA_BAD_H), 0, GCornerNone);
  
  graphics_context_set_stroke_color(ctx, GColorBlack);
#endif
}

/*
 * Function: draw_graph_part_day
 * ----------------------------------------------------------------------------
 *   Draws the graph day label part for a weekday                             
 * 
 *   ctx: the drawing context                                                 
 *   bounds_size_h: the drawing bounds height                                 
 *   s_day: string for the weekday label                                      
 *   index: the weekday position index                                        
 * ----------------------------------------------------------------------------
 */
void draw_graph_part_day(GContext *ctx, int bounds_size_h, char* s_day, int index) {
  if (index < 0) {
    index = 6; 
  }
  graphics_draw_text(ctx, s_day, s_res_font_days, 
                     GRect(s_label_points[index], 
                           bounds_size_h-GRAPH_AREA_LABELS_H, 
                           24, 
                           GRAPH_AREA_LABELS_H), 
                     GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  //graphics_draw_text(ctx, s_day, s_res_font_days, GRect(2+(20*day), (bounds_size_h/2)-GRAPH_AREA_LABELS_H, 24, GRAPH_AREA_LABELS_H), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}


/*
 * Function: draw_graph_part_base
 * ----------------------------------------------------------------------------
 *   Draws the graph curve part between the current weekday and the next day.
 *
 *   ctx: the drawing context
 *   bounds_size_h: the drawing bounds height
 *   s_day: string for the current weekday label
 *   day0: the current weekday
 *   day1: the next weekday
 *   builder: the GPathBuilder used to create the curve
 *   day_shift: used to calculate how many days day0 and day1 should be shifted
 *              by.
 *              When this is not equal to 0 it is used change at what x-
 *              coordinates the days should use.
 *   first_time: Is this the first/initial call to this function?
 * ----------------------------------------------------------------------------
 */
void draw_graph_part_base(GContext *ctx, int bounds_size_h, char* s_day, 
                          int day0, int day1, GPathBuilder *builder, 
                          int day_shift, bool first_time) {
  if (day0 > 6) {day0 -= 7;}
  if (day1 > 6) {day1 -= 7;}
  
  int mood0 = storage_get_mood(day0);
  int mood1 = storage_get_mood(day1);
  if (s_num_moods > 10) {
    // If the number of moods is greater than 10 we need to re-calculate the
    // moods to keep them within the 0 - 10 range of the graph.
    
    // Shift the moods to keep them above or equal to 0:
    // - If Minimum Mood is less than 0 then the absolute min value is added
    //   to the moods ("minus minus equals pluss")
    // - If the Minimum Mood is greater then 0 then the min value is 
    //   subtracted from the moods
    mood0 = mood0 - s_mood_min; // Shift to positive
    mood1 = mood1 - s_mood_min; // Shift to positive
    
    // Divide the mmods by a 10nth of Number of Moods. This ensures that the
    // moods always are within the 0 - 10 range of the graph.
    mood0 = mood0 / (s_num_moods / 10); // divide by 10nth of num moods
    mood1 = mood1 / (s_num_moods / 10); // divide by 10nth of num moods
  }
  
  if (day_shift != 0) {
    day0 += day_shift;
    day1 += day_shift;
    
    if (day0 < 0) { day0 += 7; } 
    else if (day0 > 6) { day0 -= 7; }
    
    if (day1 < 0) { day1 += 7; }
    else if (day1 > 6) { day1 -= 7; }
  }
  GPoint p0 = GPoint(s_day_points[day0], s_mood_points[mood0]);
  GPoint p1 = GPoint(s_day_points[day1], s_mood_points[mood1]);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "x(%d) = %d", day0, p0.x);
#ifdef PBL_PLATFORM_APLITE
  // On Original Pebble (Aplite) the graph is only drawn with separate lines 
  // and not as a bezier curve
  // This is due to the aplite platform only supports drawing closed paths 
  // (non-open)
  graphics_draw_line(ctx, p0, p1);
#elif PBL_PLATFORM_BASALT
  // On Pebble Time (Basalt) the graph is drawn as a bezier curve
  
  // Calculations to find bezier curve control points
  int c_x = (p0.x + p1.x) / 2; // Mid-point between p0 and p1
  int c_y = (p0.y + p1.y) / 2; // Mid-point between p0 and p1
  int a_x = (p0.x + c_x) / 2; // Mid-point between p0 and c
  // Find the control point based on that are furthest from the mid-point 
  // y-axis
  int a_y = math_int_find_bezier_control_point(c_y, 100); 
  int b_x = (p1.x + c_x) / 2; // Mid-point between p1 and c
  GPoint p_a = GPoint(a_x, a_y); // 1st control point between p0 and mid-point
  GPoint p_b = GPoint(b_x, a_y); // 2nd control point between mid-point and p1
  if (first_time == true) { // Move the builder to the starting point
    gpath_builder_move_to_point(builder, p0);
  }
  gpath_builder_curve_to_point(builder, p1, p_b, p_a);
#endif
  // Draw the day labels at bottom
  draw_graph_part_day(ctx, bounds_size_h, s_day, day0);
}
/*
 * Function: draw_graph_part_start
 * ----------------------------------------------------------------------------
 *   Calls "draw_graph_part_base" with the "day_shift" param se to "true".                                 *
 *
 *   Please see the draw_grap_part_base function for info about the params.
 * ----------------------------------------------------------------------------
 */
void draw_graph_part_start(GContext *ctx, int bounds_size_h, char* s_day, 
                           int day0, int day1, 
                           GPathBuilder *builder, int day_shift) {
  draw_graph_part_base(ctx, bounds_size_h, s_day, 
                       day0, day1, 
                       builder, day_shift, true);
}

/*
 * Function: draw_graph_part_parts
 * ----------------------------------------------------------------------------
 *   Calls "draw_graph_part_base" with the "day_shift" param se to "false".                                 *
 *   
 *   Please see the draw_grap_part_base function for info about the params.
 * ----------------------------------------------------------------------------
 */
void draw_graph_part_parts(GContext *ctx, int bounds_size_h, char* s_day, 
                           int day0, int day1, 
                           GPathBuilder *builder, int day_shift) {
  draw_graph_part_base(ctx, bounds_size_h, s_day, 
                       day0, day1, 
                       builder, day_shift, false);
}

/*
 * Function: create_graph
 * ----------------------------------------------------------------------------
 *   Creates the graph curve and draws labels for all weekdays.
 *   
 *   ctx: the drawing context
 *   bounds_size_h: the drawing bounds height
 *   builder: the GPathBuilder used to create the curve
 * ----------------------------------------------------------------------------
 */
void create_graph(GContext *ctx, int bounds_size_h, GPathBuilder *builder) {
#if PBL_PLATFORM_BASALT
  // On Pebble Time (Basalt) increase the width of the curve line to make it 
  // more visible
  graphics_context_set_stroke_width(ctx, 2);
#endif
  int current_day = date_get_weekday()+1;
#if APP_DEBUG
  current_day = DEBUG_DAY+1;
#endif
  if (current_day > 6) {current_day = 0;}
  int day_shift = -current_day;
  if (current_day == KEY_MOOD_SUN) {
    day_shift = current_day;
  }
  draw_graph_part_start(ctx, bounds_size_h, get_label_string(current_day), 
                        current_day, current_day+1, builder, day_shift);
  draw_graph_part_parts(ctx, bounds_size_h, get_label_string(current_day+1), 
                        current_day+1, current_day+2, builder, day_shift);
  draw_graph_part_parts(ctx, bounds_size_h, get_label_string(current_day+2), 
                        current_day+2, current_day+3, builder, day_shift);
  draw_graph_part_parts(ctx, bounds_size_h, get_label_string(current_day+3), 
                        current_day+3, current_day+4, builder, day_shift);
  draw_graph_part_parts(ctx, bounds_size_h, get_label_string(current_day+4), 
                        current_day+4, current_day+5, builder, day_shift);
  draw_graph_part_parts(ctx, bounds_size_h, get_label_string(current_day+5), 
                        current_day+5, current_day+6, builder, day_shift);
  draw_graph_part_day(ctx, bounds_size_h, get_label_string(current_day+6), 
                      KEY_MOOD_SAT);
}

/*
 * Function: canvas_update_proc
 * ----------------------------------------------------------------------------
 *   Draws the mood graph on the screen.
 *
 *   this_layer: the layer on which to draw on
 *   ctx: the drawing context
 * ----------------------------------------------------------------------------
 */
void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  if (s_path) {
    gpath_destroy(s_path);
  }
  GRect bounds = layer_get_bounds(this_layer);
  int bounds_size_w = bounds.size.w;
  int bounds_size_h = bounds.size.h;
  
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);

  // Draw a black filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  // Draw the mood lines
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  // Draw the Mood (color) coding rectangles
  draw_mood_areas(ctx, bounds_size_w);
#if PBL_PLATFORM_BASALT
  graphics_context_set_stroke_width(ctx, 1);
#endif

  // Create the mood data graph  
  create_graph(ctx, bounds_size_h, builder);
  // Set the graph line color to white
  graphics_context_set_stroke_color(ctx, GColorWhite); 
  
#ifdef PBL_PLATFORM_BASALT
  // On Pebble Time (Basalt) create path from builder
  s_path = gpath_builder_create_path(builder);
#endif
  gpath_builder_destroy(builder);
#ifdef PBL_PLATFORM_APLITE
  //gpath_draw_outline(ctx, s_path);
#elif PBL_PLATFORM_BASALT
  gpath_draw_outline_open(ctx, s_path);
#endif
  
}

/*
 * Function: window_load
 * ----------------------------------------------------------------------------
 *   Overview window load handler.
 *   Creates and inits the needed layers and sets the mood.
 *
 *   window: the window that has been loaded
 * ----------------------------------------------------------------------------
 */
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create Mood avg + median info Text Layer
  s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, TOP_TEXT_H));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, (Layer *)s_text_layer);
  set_mood();
  
  s_mood_min = storage_read_int(KEY_MOOD_MIN, NUM_MOOD_MIN);
  s_mood_max = storage_read_int(KEY_MOOD_MAX, NUM_MOOD_MAX);
  s_mood_step = storage_read_int(KEY_MOOD_STEP, NUM_MOOD_STEP);
  s_num_moods = (abs(s_mood_min) + s_mood_max);

  // Create Graph Canvas Layer
  s_canvas_layer = layer_create(GRect(0, TOP_TEXT_H, bounds.size.w, 
                                      bounds.size.h-TOP_TEXT_H));
  layer_add_child(window_layer, s_canvas_layer);
  
  // Set the Graph Layer update_proc
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
}

/*
 * Function: window_unload
 * ----------------------------------------------------------------------------
 *   Overview window unload handler.
 *   Destroys the layers and the window. Then goes back to the main window.
 *
 *   window: the window that has been unloaded
 * ----------------------------------------------------------------------------
 */
static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_text_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void overview_window_push() {
  if(!s_main_window) {
    s_res_bitham_42_light = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
    s_res_font_days = fonts_get_system_font(FONT_KEY_GOTHIC_18  );
    
    s_main_window = window_create();
  #ifdef PBL_PLATFORM_APLITE
    // window_set_fullscreen(s_main_window, true);
  #endif
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
