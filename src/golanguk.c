#include <pebble.h>
#include "schedule.h"
#include "show_session.h"
#include <time.h>

#ifdef PBL_COLOR
  #define ROW_BG_COLOR GColorClear
  #define ROW_FG_COLOR GColorBlack
  #define HEADER_BG_COLOR GColorVividCerulean
  #define HEADER_FG_COLOR GColorWhite
  #define HIGHLIGHT_BG_COLOR GColorLiberty
  #define HIGHLIGHT_FG_COLOR GColorWhite
  #define ROOM_NUMBER_COLOR GColorBabyBlueEyes
#else
  #define ROW_BG_COLOR GColorWhite
  #define ROW_FG_COLOR GColorBlack
  #define HEADER_BG_COLOR GColorBlack
  #define HEADER_FG_COLOR GColorWhite
  #define HIGHLIGHT_BG_COLOR GColorBlack
  #define HIGHLIGHT_FG_COLOR GColorWhite
#endif

static int event_day = 21;
static int event_month = 8; // August

static Window *wnd_schedule;
static MenuLayer *menu_schedule;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return schedule.num_slots;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  TimeSlot *ts = &schedule.slots[section_index];
  if (ts->is_common) {
      return 1;
  } else {
      return ts->num_sessions;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  TimeSlot *ts = &schedule.slots[cell_index->section];
  if (ts->is_small == 1) {
    return 20;
  } else {
    return 38;
  }
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  #ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, HEADER_BG_COLOR);
  graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
  graphics_context_set_text_color(ctx, HEADER_FG_COLOR);
  #endif
  menu_cell_basic_header_draw(ctx, cell_layer, schedule.slots[section_index].time);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  TimeSlot *ts = &schedule.slots[cell_index->section];
  #ifndef PBL_COLOR
  graphics_context_set_text_color(ctx, ROW_FG_COLOR);
  graphics_context_set_fill_color(ctx, ROW_BG_COLOR);
  #endif
  if (ts->is_common) {
      graphics_draw_text(ctx, ts->title, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
              layer_get_bounds(cell_layer), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  } else {
      GRect title_bounds = layer_get_bounds(cell_layer);
      title_bounds.origin.x += 30;
      title_bounds.size.w -= 30;
      title_bounds.origin.y += 3;
      title_bounds.size.h -= 3;
      GRect icon_bounds = layer_get_bounds(cell_layer);
      icon_bounds.size.w = 24;
      icon_bounds.size.h = 28;
      Session *s = &(ts->sessions[cell_index->row]); 
      graphics_draw_text(ctx, s->title, fonts_get_system_font(FONT_KEY_GOTHIC_14), title_bounds,
              GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
      char *room_number_str = "0";
      snprintf(room_number_str, 2, "%d", s->room);

      #ifdef PBL_COLOR
      GFont room_font = fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM);
      graphics_context_set_text_color(ctx, ROOM_NUMBER_COLOR);
      #else
      GFont room_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
      #endif
      graphics_draw_text(ctx, room_number_str, room_font, icon_bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  TimeSlot *ts = &schedule.slots[cell_index->section];
  if (!ts->is_common) {
    Session *s = &(ts->sessions[cell_index->row]); 
    show_session(s);
  }
}

MenuIndex get_current_session_from_time() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    MenuIndex current = {.section = 0, .row = 0};
    if (tm_now->tm_mon != event_month - 1 || tm_now->tm_mday != event_day) {
        return current;
    }
    for(int ts_idx=0; ts_idx<schedule.num_slots; ts_idx++) {
        TimeSlot *ts = &schedule.slots[ts_idx];
        if (tm_now->tm_hour < ts->start_hour) {
            break;
        } else if(tm_now->tm_hour == ts->start_hour && tm_now->tm_min < ts->start_min) {
            break;
        }
        current.section = ts_idx;
    }
    return current;
}

MenuIndex get_current_session_from_args() {
    int args = launch_get_args();
    MenuIndex current = {.section = args / 10, .row = args % 10};
    return current;
}

MenuIndex get_current_session() {
    #ifdef PBL_COLOR
    if (launch_reason() == APP_LAUNCH_TIMELINE_ACTION && launch_get_args() > 0) {
        return get_current_session_from_args();
    }
    #endif
    return get_current_session_from_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  menu_schedule = menu_layer_create(bounds);

  #ifdef PBL_COLOR
  menu_layer_set_normal_colors(menu_schedule, ROW_BG_COLOR, ROW_FG_COLOR);
  menu_layer_set_highlight_colors(menu_schedule, HIGHLIGHT_BG_COLOR, HIGHLIGHT_FG_COLOR);
  #endif

  menu_layer_set_callbacks(menu_schedule, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(menu_schedule, window);
  MenuIndex idx = get_current_session();
  menu_layer_set_selected_index(menu_schedule, idx, MenuRowAlignCenter, false);
  layer_add_child(window_layer, menu_layer_get_layer(menu_schedule));
}

static void main_window_unload(Window *window) {
  menu_layer_destroy(menu_schedule);
}

static void init() {
  init_session_window();
  init_schedule();
  wnd_schedule = window_create();
#ifndef PBL_COLOR
  window_set_fullscreen(wnd_schedule, true);
#endif
  window_set_window_handlers(wnd_schedule, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(wnd_schedule, true);
}

static void deinit() {
  window_destroy(wnd_schedule);
  deinit_session_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
