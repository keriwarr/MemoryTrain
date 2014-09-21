#include "pebble.h"
#include "pebble_fonts.h"
#include "qa.h"
 
static Window *window;

static TextLayer *question_layer;

static Layer *window_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

//
char buffer[4];
//

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
    layer_set_frame((Layer*)question_layer,GRect(0,((168-(text_layer_get_content_size(question_layer).h))/2)-4,112,text_layer_get_content_size(question_layer).h));
    
    //
    int value = text_layer_get_content_size(question_layer).h;
    snprintf(buffer, 4, "%d", value);
    //text_layer_set_text(question_layer, buffer);
    //
    

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(question_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(question_layer);    
    layer_destroy(window_layer);
    // Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);

    // Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);
}

static void init(void) {
  window = window_create();
    window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);
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
