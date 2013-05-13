/*
  Military Text
  Inspired by TextWatch
  With Date, 12H display + second ticker and Week #
  Hour and Min has inversed background
  With Animation!
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "english_time.h"

#define MY_UUID { 0xD1, 0x0B, 0x15, 0xB9, 0x76, 0xE7, 0x45, 0x91, 0xAB, 0x89, 0x30, 0x57, 0x59, 0xB6, 0x42, 0xF3 }
PBL_APP_INFO(MY_UUID,
             "Military Text", "atpeaz.com",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);
#define ANIMATION_DURATION 800
#define LINE_BUFFER_SIZE 50
#define WINDOW_NAME "military_text"

Window window;

typedef struct {
  TextLayer layer[2];
  PropertyAnimation layer_animation[2];
} TextLine;

typedef struct {
  char hour1[LINE_BUFFER_SIZE];
  char hour2[LINE_BUFFER_SIZE];
  char min1[LINE_BUFFER_SIZE];
  char min2[LINE_BUFFER_SIZE];
} TheTime;

TextLayer topbarLayer;
TextLayer bottombarLayer;
TextLayer white_bg;
TextLine line1;
TextLine line2;
TextLine line3;
TextLine line4;

static TheTime cur_time;
static TheTime new_time;

static char str_topbar[LINE_BUFFER_SIZE];
static char str_bottombar[LINE_BUFFER_SIZE];
static bool busy_animating_in = false;
static bool busy_animating_out = false;
const int hour1_y = 10;
const int hour2_y = 35;
const int min1_y = 75;
const int min2_y = 102;
const int textline_size = 50;

GFont text_font;
GFont text_font_light;
GFont bar_font;

void animationInStoppedHandler(struct Animation *animation, bool finished, void *context) {
  busy_animating_in = false;
  //reset cur_time
  cur_time = new_time;
}

void animationOutStoppedHandler(struct Animation *animation, bool finished, void *context) {
  //reset out layer to x=144
  TextLayer *outside = (TextLayer *)context;
  GRect rect = layer_get_frame(&outside->layer);
  rect.origin.x = 144;
  layer_set_frame(&outside->layer, rect);

  busy_animating_out = false;
}

void updateLayer(TextLine *animating_line, int line) {
  
  TextLayer *inside, *outside;
  GRect rect = layer_get_frame(&animating_line->layer[0].layer);

  inside = (rect.origin.x == 0) ? &animating_line->layer[0] : &animating_line->layer[1];
  outside = (inside == &animating_line->layer[0]) ? &animating_line->layer[1] : &animating_line->layer[0];

  GRect in_rect = layer_get_frame(&outside->layer);
  GRect out_rect = layer_get_frame(&inside->layer);

  in_rect.origin.x -= 144;
  out_rect.origin.x -= 144;

 //animate out current layer
  busy_animating_out = true;
  property_animation_init_layer_frame(&animating_line->layer_animation[1], &inside->layer, NULL, &out_rect);
  animation_set_duration(&animating_line->layer_animation[1].animation, ANIMATION_DURATION);
  animation_set_curve(&animating_line->layer_animation[1].animation, AnimationCurveEaseOut);
  animation_set_handlers(&animating_line->layer_animation[1].animation, (AnimationHandlers) {
    .stopped = (AnimationStoppedHandler)animationOutStoppedHandler
  }, (void *)inside);
  animation_schedule(&animating_line->layer_animation[1].animation);

  if (line==1){
    text_layer_set_text(outside, new_time.hour1);
    text_layer_set_text(inside, cur_time.hour1);
  }
  if (line==2){
    text_layer_set_text(outside, new_time.hour2);
    text_layer_set_text(inside, cur_time.hour2);
  }
  if (line==3){
    text_layer_set_text(outside, new_time.min1);
    text_layer_set_text(inside, cur_time.min1);
  }
  if (line==4){
    text_layer_set_text(outside, new_time.min2);
    text_layer_set_text(inside, cur_time.min2);
  }
  
  //animate in new layer
  busy_animating_in = true;
  property_animation_init_layer_frame(&animating_line->layer_animation[0], &outside->layer, NULL, &in_rect);
  animation_set_duration(&animating_line->layer_animation[0].animation, ANIMATION_DURATION);
  animation_set_curve(&animating_line->layer_animation[0].animation, AnimationCurveEaseOut);
  animation_set_handlers(&animating_line->layer_animation[0].animation, (AnimationHandlers) {
    .stopped = (AnimationStoppedHandler)animationInStoppedHandler
  }, (void *)outside);
  animation_schedule(&animating_line->layer_animation[0].animation);
}

void update_watch(PblTm* t) {
  //Let's get the new time and date
  military_time_4lines(t->tm_hour, t->tm_min, new_time.hour1, new_time.hour2, new_time.min1, new_time.min2);
  string_format_time(str_topbar, sizeof(str_topbar), "%A | %e %b", t);
  string_format_time(str_bottombar, sizeof(str_bottombar), " %r | Week %W", t);
  
  //Let's update the top and bottom bar anyway - **to optimize later to only update top bar every new day.
  text_layer_set_text(&topbarLayer, str_topbar);
  text_layer_set_text(&bottombarLayer, str_bottombar);

  //update hour1 only if changed
  if(strcmp(new_time.hour1,cur_time.hour1) != 0){
    updateLayer(&line1, 1);
  }
    //update hour2 only if changed
  if(strcmp(new_time.hour2,cur_time.hour2) != 0){
    updateLayer(&line2, 2);
  }
  //update min1 only if changed
  if(strcmp(new_time.min1,cur_time.min1) != 0){
    updateLayer(&line3, 3);
  }
  //update min2 only if changed happens on
  if(strcmp(new_time.min2,cur_time.min2) != 0){
    updateLayer(&line4, 4);
  }
}

void init_watch(PblTm* t) {
  //Let's get the new time and date
  military_time_4lines(t->tm_hour, t->tm_min, cur_time.hour1, cur_time.hour2, cur_time.min1, cur_time.min2);
  string_format_time(str_topbar, sizeof(str_topbar), "%A | %e %b", t);
  string_format_time(str_bottombar, sizeof(str_bottombar), " %r | Week %W", t);
  
  //Let's update the top and bottom bar anyway - **to optimize later to only update top bar every new day.
  text_layer_set_text(&topbarLayer, str_topbar);
  text_layer_set_text(&bottombarLayer, str_bottombar);
  text_layer_set_text(&line1.layer[0], cur_time.hour1);
  text_layer_set_text(&line2.layer[0], cur_time.hour2);
  text_layer_set_text(&line3.layer[0], cur_time.min1);
  text_layer_set_text(&line4.layer[0], cur_time.min2);

  new_time = cur_time;
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  // Create our app's base window
  window_init(&window, WINDOW_NAME);
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  // Init the text layers used to show the time 
  text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TWCENMT_40_BOLD));
  text_font_light = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TWCENMT_38));
  //text_font_light = fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK);
  bar_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

  // white_bg
  text_layer_init(&white_bg, GRect(0, hour1_y, 144, (83 - hour1_y)));
  text_layer_set_background_color(&white_bg, GColorWhite);

  // hour1
  text_layer_init(&line1.layer[0], GRect(0, hour1_y, 144, textline_size));
  text_layer_set_text_color(&line1.layer[0], GColorBlack);
  text_layer_set_background_color(&line1.layer[0], GColorClear);
  text_layer_set_font(&line1.layer[0], text_font);
  text_layer_set_text_alignment(&line1.layer[0], GTextAlignmentLeft);
  
  text_layer_init(&line1.layer[1], GRect(144, hour1_y, 144, textline_size));
  text_layer_set_text_color(&line1.layer[1], GColorBlack);
  text_layer_set_background_color(&line1.layer[1], GColorClear);
  text_layer_set_font(&line1.layer[1], text_font);
  text_layer_set_text_alignment(&line1.layer[1], GTextAlignmentLeft);

  // hour2
  text_layer_init(&line2.layer[0], GRect(0, hour2_y, 144, textline_size));
  text_layer_set_text_color(&line2.layer[0], GColorBlack);
  text_layer_set_background_color(&line2.layer[0], GColorClear);
  text_layer_set_font(&line2.layer[0], text_font);
  text_layer_set_text_alignment(&line2.layer[0], GTextAlignmentLeft);
  
  text_layer_init(&line2.layer[1], GRect(144, hour2_y, 144, textline_size));
  text_layer_set_text_color(&line2.layer[1], GColorBlack);
  text_layer_set_background_color(&line2.layer[1], GColorClear);
  text_layer_set_font(&line2.layer[1], text_font);
  text_layer_set_text_alignment(&line2.layer[1], GTextAlignmentLeft);

  // min1
  text_layer_init(&line3.layer[0], GRect(0, min1_y, 144, textline_size));
  text_layer_set_text_color(&line3.layer[0], GColorWhite);
  text_layer_set_background_color(&line3.layer[0], GColorClear);
  text_layer_set_font(&line3.layer[0], text_font_light);
  text_layer_set_text_alignment(&line3.layer[0], GTextAlignmentLeft);

  text_layer_init(&line3.layer[1], GRect(144, min1_y, 144, textline_size));
  text_layer_set_text_color(&line3.layer[1], GColorWhite);
  text_layer_set_background_color(&line3.layer[1], GColorClear);
  text_layer_set_font(&line3.layer[1], text_font_light);
  text_layer_set_text_alignment(&line3.layer[1], GTextAlignmentLeft);
  
  // min2
  text_layer_init(&line4.layer[0], GRect(0, min2_y, 144, textline_size));
  text_layer_set_text_color(&line4.layer[0], GColorWhite);
  text_layer_set_background_color(&line4.layer[0], GColorClear);
  text_layer_set_font(&line4.layer[0], text_font_light);
  text_layer_set_text_alignment(&line4.layer[0], GTextAlignmentLeft);

  text_layer_init(&line4.layer[1], GRect(144, min2_y, 144, textline_size));
  text_layer_set_text_color(&line4.layer[1], GColorWhite);
  text_layer_set_background_color(&line4.layer[1], GColorClear);
  text_layer_set_font(&line4.layer[1], text_font_light);
  text_layer_set_text_alignment(&line4.layer[1], GTextAlignmentLeft);

  // top_bar
  text_layer_init(&topbarLayer, GRect(0, 0, 144, 18));
  text_layer_set_text_color(&topbarLayer, GColorWhite);
  text_layer_set_background_color(&topbarLayer, GColorBlack);
  text_layer_set_font(&topbarLayer, bar_font);
  text_layer_set_text_alignment(&topbarLayer, GTextAlignmentCenter);

  // bottom_bar
  text_layer_init(&bottombarLayer, GRect(0, 150, 144, 18));
  text_layer_set_text_color(&bottombarLayer, GColorBlack);
  text_layer_set_background_color(&bottombarLayer, GColorWhite);
  text_layer_set_font(&bottombarLayer, bar_font);
  text_layer_set_text_alignment(&bottombarLayer, GTextAlignmentCenter);

  PblTm t;
  get_time(&t);
  init_watch(&t);

  layer_add_child(&window.layer, &white_bg.layer);
  layer_add_child(&window.layer, &line4.layer[0].layer);
  layer_add_child(&window.layer, &line4.layer[1].layer);  
  layer_add_child(&window.layer, &line3.layer[0].layer);
  layer_add_child(&window.layer, &line3.layer[1].layer); 
  layer_add_child(&window.layer, &line2.layer[0].layer);
  layer_add_child(&window.layer, &line2.layer[1].layer);
  layer_add_child(&window.layer, &line1.layer[0].layer);
  layer_add_child(&window.layer, &line1.layer[1].layer);
  layer_add_child(&window.layer, &bottombarLayer.layer); 
  layer_add_child(&window.layer, &topbarLayer.layer);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  // Fonts
  fonts_unload_custom_font(text_font);
  fonts_unload_custom_font(text_font_light);
}

// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  if (busy_animating_out || busy_animating_in) return;

  update_watch(t->tick_time);  
}

// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}