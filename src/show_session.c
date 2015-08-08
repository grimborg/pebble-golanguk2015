#include "pebble.h"
#include "schedule.h"

Window *wnd_session;
Layer *layer_bottom_bar;
TextLayer *layer_session_title;
TextLayer *layer_session_presenter;
TextLayer *layer_session_room;
TextLayer *layer_session_start;
Session *current_session;

#define REPEAT_INTERVAL_MS 1000

#ifdef PBL_COLOR
#define PRESENTER_COLOR GColorLiberty
#define TIME_COLOR GColorWhite
#define ROOM_COLOR GColorBabyBlueEyes
#define BOTTOM_BAR_COLOR GColorLiberty
#else
#define TIME_COLOR GColorWhite
#define ROOM_COLOR GColorWhite
#define BOTTOM_BAR_COLOR GColorBlack
#define PRESENTER_COLOR GColorBlack
#endif

#ifdef PBL_COLOR
static char *room_names[3] = {"", "1", "2"};
#else
static char *room_names[3] = {"", "Room 1", "Room 2"};
#endif

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
    if (layer_bottom_bar) {
        layer_destroy(layer_bottom_bar);
    }
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

static void update_layer_bottom_bar(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, BOTTOM_BAR_COLOR);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

void init_session_window() {
    wnd_session = window_create();
#ifndef PBL_COLOR
    window_set_fullscreen(wnd_session, true);
#endif
    layer_session_presenter = text_layer_create((GRect(2, 0, 140, 30)));
    text_layer_set_background_color((layer_session_presenter), GColorClear);
    text_layer_set_text_color((layer_session_presenter), PRESENTER_COLOR);
#ifdef PBL_COLOR
    text_layer_set_font(layer_session_presenter, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
#else
    text_layer_set_font(layer_session_presenter, fonts_get_system_font(FONT_KEY_GOTHIC_24));
#endif
    text_layer_set_text_alignment(layer_session_presenter, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_presenter));

    layer_session_title = text_layer_create((GRect(2, 23, 140, 100)));
    text_layer_set_background_color((layer_session_title), GColorClear);
    text_layer_set_text_color((layer_session_title), GColorBlack);
    text_layer_set_font(layer_session_title, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(layer_session_title, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_title));

layer_bottom_bar = layer_create(GRect(0, 105, 144, 63));
layer_set_update_proc(layer_bottom_bar, update_layer_bottom_bar);
layer_add_child(window_get_root_layer(wnd_session), layer_bottom_bar);
#ifdef PBL_COLOR
    layer_session_start = text_layer_create((GRect(2, 120, 134, 30)));
    text_layer_set_background_color((layer_session_start), GColorClear);
    text_layer_set_text_color((layer_session_start), TIME_COLOR);
    text_layer_set_font(layer_session_start, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
    text_layer_set_text_alignment(layer_session_start, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_start));

    layer_session_room = text_layer_create((GRect(10, 120, 134, 30)));
    text_layer_set_background_color((layer_session_room), GColorClear);
    text_layer_set_text_color((layer_session_room), ROOM_COLOR);
    text_layer_set_font(layer_session_room, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
    text_layer_set_text_alignment(layer_session_room, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_room));
#else
    layer_session_start = text_layer_create((GRect(10, 120, 124, 30)));
    text_layer_set_background_color((layer_session_start), GColorClear);
    text_layer_set_text_color((layer_session_start), TIME_COLOR);
    text_layer_set_font(layer_session_start, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(layer_session_start, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_start));

    layer_session_room = text_layer_create((GRect(10, 120, 124, 30)));
    text_layer_set_background_color((layer_session_room), GColorClear);
    text_layer_set_text_color((layer_session_room), ROOM_COLOR);
    text_layer_set_font(layer_session_room, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(layer_session_room, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(wnd_session), text_layer_get_layer(layer_session_room));
#endif
    window_set_click_config_provider(wnd_session, click_config_provider);
}

