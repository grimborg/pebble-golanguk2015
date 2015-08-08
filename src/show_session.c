#include "pebble.h"
#include "schedule.h"

Window *wnd_session;
TextLayer *layer_session_title;
TextLayer *layer_session_presenter;
TextLayer *layer_session_room;
TextLayer *layer_session_start;
Session *current_session;

#define REPEAT_INTERVAL_MS 1000

#ifdef PBL_COLOR
#define PRESENTER_COLOR GColorDukeBlue
#else
#define PRESENTER_COLOR GColorBlack
#endif

static char *room_names[5] = {"", "Room 1", "Room 2"};

static void update_text_layers() {
    text_layer_set_text(layer_session_title, current_session->title);
    text_layer_set_text(layer_session_presenter, current_session->presenter);
    text_layer_set_text(layer_session_room, room_names[current_session->room]);
    text_layer_set_text(layer_session_start, current_session->start);
}


void show_session(Session *session) {
    current_session = session;
    update_text_layers();
    window_stack_push(wnd_session, true);
}

void deinit_session_window() {
    text_layer_destroy(layer_session_title);
    text_layer_destroy(layer_session_presenter);
    text_layer_destroy(layer_session_room);
    text_layer_destroy(layer_session_start);
    window_destroy(wnd_session);
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (current_session->prev) {
        current_session = current_session->prev;
        update_text_layers();
    }
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (current_session->next) {
        current_session = current_session->next;
        update_text_layers();
    }
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    window_stack_remove(wnd_session, true);
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

void init_session_window() {
    wnd_session = window_create();

    layer_session_presenter = text_layer_create((GRect(2, 0, 140, 30)));
    text_layer_set_background_color((layer_session_presenter), GColorClear);
    text_layer_set_text_color((layer_session_presenter), PRESENTER_COLOR);
    text_layer_set_font(layer_session_presenter, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(layer_session_presenter, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_presenter));

    layer_session_title = text_layer_create((GRect(2, 23, 140, 100)));
    text_layer_set_background_color((layer_session_title), GColorClear);
    text_layer_set_text_color((layer_session_title), GColorBlack);
    text_layer_set_font(layer_session_title, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(layer_session_title, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_title));

    layer_session_start = text_layer_create((GRect(2, 130, 140, 20)));
    text_layer_set_background_color((layer_session_start), GColorClear);
    text_layer_set_text_color((layer_session_start), GColorBlack);
    text_layer_set_font(layer_session_start, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(layer_session_start, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_start));

    layer_session_room = text_layer_create((GRect(2, 130, 140, 20)));
    text_layer_set_background_color((layer_session_room), GColorClear);
    text_layer_set_text_color((layer_session_room), GColorBlack);
    text_layer_set_font(layer_session_room, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(layer_session_room, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_room));

    window_set_click_config_provider(wnd_session, click_config_provider);
}

