#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xA7, 0x1B, 0xB0, 0xE2, 0xD2, 0x62, 0x4E, 0xA6, 0xAA, 0x30, 0xED, 0xBE, 0x01, 0xE3, 0x8A, 0x03}
PBL_APP_INFO(MY_UUID, "circletick2", "Pebble Technology", 3, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_day_layer;

Layer minute_display_layer;

Layer line_layer;


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-5.04, -48}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per minute;
    {5.04,  -48},
  }
};
/*
GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {35, 20}, // point
    {35, 40}, // bottom right
    {30, 40}, // bottom left
  }
};
*/
GPath minute_segment_path;

void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  PblTm t;
  get_time(&t);

// fill white...
  graphics_context_set_fill_color(ctx, GColorWhite);
//----------------
	unsigned int cut = t.tm_sec * 2;

  /*graphics_draw_circle(ctx, GPoint(120,30), 10);
  graphics_fill_circle(ctx, GPoint(120,30), 10);
  graphics_draw_circle(ctx, GPoint(120,70), 10);*/

  unsigned int angle = t.tm_sec * 6;

	GPoint center = GPoint(35,20);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 18);
	graphics_context_set_fill_color(ctx, GColorBlack);

  for(; angle < 355; angle += 1) {
    gpath_rotate_to(&minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);
    gpath_draw_filled(ctx, &minute_segment_path);
  }
	graphics_fill_circle(ctx, center, 16);
  //gpath_rotate_to(&minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_circle(ctx, center, 8);
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //gpath_draw_filled(ctx, &minute_segment_path);

}


int black = 0; // for the blinking circle. should it be black or not (therefore white)?

void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  	graphics_context_set_stroke_color(ctx, GColorWhite);
		graphics_context_set_fill_color(ctx, GColorWhite);

//LINE
  //graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));

}
/*
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t; // TODO: Pass the time direct to the layers?
  (void)ctx;

  layer_mark_dirty(&minute_display_layer);
 // layer_mark_dirty(&hour_display_layer);
}
*/


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "circletick2");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);
//DATE
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(11, 63, 144-11, 168-63));
  //text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
	text_layer_set_font(&text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(&window.layer, &text_date_layer.layer);

//TIME
  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  //layer_set_frame(&text_time_layer.layer, GRect(7, 8, 10, 11)); // x,y,w,h
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);

//DAY
  text_layer_init(&text_day_layer, window.layer.frame);
  text_layer_set_text_color(&text_day_layer, GColorWhite);
  text_layer_set_background_color(&text_day_layer, GColorClear);
  layer_set_frame(&text_day_layer.layer, GRect(7, 42, 144-7, 168-42));
  //layer_set_frame(&text_day_layer.layer, GRect(50, 50, 10, 10));
  //text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UNICONS_30)));
	text_layer_set_font(&text_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(&window.layer, &text_day_layer.layer);

  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);

// CIRCLE 
  // Init the layer for the minute display
  layer_init(&minute_display_layer, window.layer.frame);
  layer_set_frame(&minute_display_layer, GRect(72, 15, 70, 40));

  minute_display_layer.update_proc = &minute_display_layer_update_callback;
  layer_add_child(&window.layer, &minute_display_layer);
  // Init the minute segment path
  gpath_init(&minute_segment_path, &MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(&minute_segment_path, GPoint(35,20));
  //gpath_move_to(&minute_segment_path, grect_center_point(&minute_display_layer.frame));
  

  // TODO: Update display here to avoid blank display on launch?
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;
//-----------------
//-----------------

 (void)t; // TODO: Pass the time direct to the layers?
  (void)ctx;

  layer_mark_dirty(&minute_display_layer);
 // layer_mark_dirty(&hour_display_layer);



  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Wed September 99";
  //static char day_text[] = "XXX XXXXXXXXX 00";

  char *time_format;
/*
//DAY
  //string_format_time(day_text, sizeof(day_text), "%a", t->tick_time);
  text_layer_set_text(&text_day_layer, day_text);
*/
//DATE
  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%a %B %e", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);
  if(black==0){black=1;}else{black=0;}
  layer_mark_dirty(&line_layer);

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
