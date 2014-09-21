#include <pebble.h>

#define KEY_QUESTION 0
#define KEY_ANSWER 1
#define KEY_SCORE 2

 static const char *QA[] = {
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
static const char *current_question;
static const char *current_answer;

static Window *window;
static TextLayer *prompt_layer;
static Layer *window_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

bool question_answered = false;

static void update_text() {
    if (question_answered)
        text_layer_set_text(prompt_layer, current_answer);
    else
        text_layer_set_text(prompt_layer, current_question);
}

static void update_time() {
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    // set the time of the buffer
    static char buffer[] = "answer\n\n00:00";
    if(clock_is_24h_style() == true) // 24 hour format
        strftime(buffer, sizeof("answer\n\n00:00"), "answer\n\n%H:%M", tick_time);
    else // 12 hour format
        strftime(buffer, sizeof("answer\n\n00:00"), "answer\n\n%I:%M", tick_time);
    text_layer_set_text(prompt_layer, buffer);
}

// called every minute
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_text();
    if (question_answered) update_time();
}

// called upon receiving an AppMessage from the phone
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *t = dict_read_first(iterator);
    while(t != NULL) {
        switch(t->key) {
            case KEY_QUESTION:
                current_question = t->value;
                break;
            case KEY_ANSWER:
                current_answer = t->value;
                break;
            case KEY_SCORE:
                // do nothing with the score
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        t = dict_read_next(iterator);
    }
    question_answered = false;
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

// called upon clicking the up button
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    layer_set_hidden(bitmap_layer_get_layer(s_background_layer), true);
    layer_set_frame(text_layer_get_layer(prompt_layer), GRect(0,6,144,84));
    question_answered = true;
    update_text();
    update_time();
    
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_cstring(iter, KEY_STATUS, "remembered");
    app_message_outbox_send();
}

// called upon clicking the down button
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    layer_set_hidden(bitmap_layer_get_layer(s_background_layer), true);
    layer_set_frame(text_layer_get_layer(prompt_layer), GRect(0,6,144,84));
    question_answered = true;
    update_text();
    update_time();
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
    current_question = QA[4];
    
    window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);    
    GRect inner = GRect(0,0,112,168);
    
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_background);
    s_background_layer = bitmap_layer_create(bounds);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    prompt_layer = text_layer_create(inner);
    text_layer_set_text_color(prompt_layer, GColorBlack);
    text_layer_set_background_color(prompt_layer, GColorClear);
    text_layer_set_font(prompt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(prompt_layer, GTextAlignmentCenter);
    layer_set_frame(
        text_layer_get_layer(prompt_layer),
        GRect(0, ((168 - (text_layer_get_content_size(prompt_layer).h)) / 2) - 4, 112, text_layer_get_content_size(prompt_layer).h)
    );
    layer_add_child(window_layer, text_layer_get_layer(prompt_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(prompt_layer);  
    layer_destroy(window_layer);
    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
}

static void init(void) {
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
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
