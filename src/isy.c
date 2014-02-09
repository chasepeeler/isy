#include <pebble.h>


static Window *window;
static TextLayer *text_layer;
static TextLayer *status_layer;
static ActionBarLayer *action_bar;

static AppSync sync;
static uint8_t sync_buffer[64];

static GBitmap *powerBitmap;

enum IsyKey {
  STATUS_KEY = 0x0,         // TUPLE_CSTRING
  COMMAND_KEY = 0x1, // TUPLE_INT

};

static void send_cmd(int cmd) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending Command: %d", cmd);
  Tuplet cmd_tuple = TupletInteger(COMMAND_KEY, cmd);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &cmd_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Tuple Change");
	static char status_message[] = "Current Status: Off";
	switch (key) {
    case STATUS_KEY:
	  snprintf(status_message, sizeof(status_message), "Current Status: %s",new_tuple->value->cstring);
      text_layer_set_text(status_layer, status_message);
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "New Status: %s", new_tuple->value->cstring);
      break;

  }
}




static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
 	send_cmd(1);
}
/*
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
text_layer_set_text(status_layer, "Turning On...");  
APP_LOG(APP_LOG_LEVEL_DEBUG, "On Requested");
	send_cmd(1);
	
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(status_layer, "Turning Off...");
APP_LOG(APP_LOG_LEVEL_DEBUG, "Off Requested");
	send_cmd(0);
}
*/
void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  //GRect bounds = layer_get_bounds(window_layer);

	// Initialize the action bar:
  action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar,
                                             click_config_provider);
  
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, powerBitmap);
  const int16_t width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;
   const int16_t height = layer_get_frame(window_layer).size.h;
  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { width, height/2 } });
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(text_layer, "Den Lamp");
//  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
   status_layer = text_layer_create(GRect(0, height/2, width, height/2));
 // text_layer_set_text_color(status_layer, GColorWhite);
 // text_layer_set_background_color(status_layer, GColorClear);
  text_layer_set_font(status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
//  text_layer_set_text_alignment(status_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(status_layer));
	
	Tuplet initial_values[] = {
    	TupletCString(STATUS_KEY, "NA"),
		TupletInteger(COMMAND_KEY, 0)
    };
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
    sync_tuple_changed_callback, sync_error_callback, NULL);

	
}

static void window_unload(Window *window) {
	app_sync_deinit(&sync);
	text_layer_destroy(text_layer);
	text_layer_destroy(status_layer);
	action_bar_layer_destroy(action_bar);

}

static void init(void) {
	powerBitmap = gbitmap_create_with_resource(RESOURCE_ID_POWER_ICON);
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
	
	const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
	
  const bool animated = true;
  window_stack_push(window, animated);

	
}

static void deinit(void) {
  window_destroy(window);
	gbitmap_destroy(powerBitmap);
}



int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
