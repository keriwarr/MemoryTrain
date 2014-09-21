#include <pebble.h>
    
 static const char *QA [] = {
    "What year was the University of Waterloo established?",
    "1957",
    "What is everyone's favourite hackathon?",
    "Hack the North",
    "What is the airspeed of an unladen swallow?",
    "11 m/s (European)",
    "How fast did Han Solo make the Kessel Run?",
    "12 Parsecs",
    "Who created XKCD?",
    "Randall Munroe",
    "Who played the Tenth Doctor?",
    "David Tennant",
};

static Window *window;
static TextLayer *question_layer;
static TextLayer *time_layer;
static Layer *window_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

bool answered = false;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "answer\n\n00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("answer\n\n00:00"), "answer\n\n%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("answer\n\n00:00"), "answer\n\n%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(question_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (answered) update_time();
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    layer_set_hidden(bitmap_layer_get_layer(s_background_layer), true);
    layer_set_frame(text_layer_get_layer(question_layer), GRect(0,6,144,84));
    text_layer_set_text(question_layer, QA[5]);
    answered = true;
    update_time();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    layer_set_hidden(bitmap_layer_get_layer(s_background_layer), true);
    layer_set_frame(text_layer_get_layer(question_layer), GRect(0,6,144,84));
    text_layer_set_text(question_layer, QA[5]);
    answered = true;
    update_time();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {  
    
    window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);    
    GRect inner = GRect(0,0,112,168);
    
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_background);
    s_background_layer = bitmap_layer_create(bounds);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    question_layer = text_layer_create(inner);
    text_layer_set_text_color(question_layer, GColorBlack);
    text_layer_set_text(question_layer, QA[4]);
    text_layer_set_background_color(question_layer, GColorClear);
    text_layer_set_font(question_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(question_layer, GTextAlignmentCenter);
    layer_set_frame(text_layer_get_layer(question_layer),GRect(0,((168-(text_layer_get_content_size(question_layer).h))/2)-4,112,text_layer_get_content_size(question_layer).h));
    layer_add_child(window_layer, text_layer_get_layer(question_layer));  
    
    
}

static void window_unload(Window *window) {
    text_layer_destroy(question_layer);  
    layer_destroy(window_layer);
    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
}

static void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_fullscreen(window, true);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
    window_stack_push(window, true /* Animated */);
    
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    window_set_background_color(window, GColorWhite);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}