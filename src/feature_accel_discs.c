#include "pebble.h"
#include "qa.c"
 
static Window *window;

static void window_load(Window *window) {  
    
    static TextLayer *question_layer;
    GRect bounds = layer_get_frame(window_layer);
    question_layer = text_layer_create(bounds);
    text_layer_set_background_color(question_layer, GColorClear);
    text_layer_set_text_color(question_layer, GColorBlack);
    text_layer_set_text(question_layer, QA[0]);

    text_layer_set_font(question_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(question_layer, GTextAlignmentCenter);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(question_layer));
}

static void window_unload(Window *window) {
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);
}

static void deinit(void) {
  window_destroy(window);
    text_layer_destroy(question_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
