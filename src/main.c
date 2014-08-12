#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
int num_of_strokes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0,    // there is no hole 0
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num_of_putts[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int current_hole = 1; // initialize to first hole
int holes_in_round = 18;
bool next_shot_is_tee_shot = false; // assigned value of true after a putt is made
bool round_complete = false;

char clubs_selected[200];

/////// Stroke manipulation functions
static void add_stroke(char shot_type) {
  if (shot_type == 'p') {
    num_of_putts[current_hole]++;
  }
  num_of_strokes[current_hole]++;
}

static void subtract_stroke() {
  num_of_strokes[current_hole]--;
  if(num_of_strokes[current_hole] < 0) {
    num_of_strokes[current_hole] = 0;    
  }
}

static void next_hole() {
  if (current_hole == holes_in_round) {
    current_hole = 1;
  }
  else {
    current_hole++;
  }
}
static void prev_hole() {
  if (current_hole == 1) {
    current_hole = holes_in_round;
  }
  else {
    current_hole--;
  }
}
/////// Update window
static void add_and_show_total() { // add up strokes and putts and show them
  int total_score = 0;
  int total_putts = 0;
  for (int a=1; a<holes_in_round+1; a++) {
    total_score+=num_of_strokes[a];
    total_putts+=num_of_putts[a];
  }
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "You shot: %u\n%u Putts", total_score, total_putts);
  text_layer_set_text(text_layer, body_text);
  
  round_complete = true;
}

static void show_club_selection() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  Drive  S%u\n----------\nApproach\n----------\nPutt", current_hole, num_of_strokes[current_hole]);
  text_layer_set_text(text_layer, body_text);
}

/////// Click event functions
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) { // undo last hole
  round_complete = false;
  num_of_strokes[current_hole] = 0;
  next_shot_is_tee_shot = false;
  show_club_selection();
}
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  //... called when long click is released ...
  // do nothing
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    if (next_shot_is_tee_shot == true) { // proceed to next hole if last stroke was a putt
      if (current_hole == holes_in_round) { // if select is pressed after putting out the final hole
        add_and_show_total();
      }
      else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('d');
        show_club_selection();
      }
    }
    else { // stay in the same hole if there are no putts yet in this hole
      add_stroke('d');
      show_club_selection();
    }
  }
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    if (next_shot_is_tee_shot == true) { // go to next next if last stroke was a putt
      if (current_hole == holes_in_round) { // if select is pressed after putting out the final hole
        add_and_show_total();
      }
      else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('a');
        show_club_selection();
      }
    }
    else {  // stay in the same hole if there are no putts yet in this hole
      add_stroke('a');
      show_club_selection();
    }
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    add_stroke('p');
    show_club_selection();
    next_shot_is_tee_shot = true;
  }
}

/////////// Configure event handlers, windows/text layers and other stuff

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  text_layer = text_layer_create(GRect(0, 0, 144, 154));  
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  Drive  S%u\n----------\nApproach\n----------\nPutt", current_hole, num_of_strokes[current_hole]);
  text_layer_set_text(text_layer, body_text);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
  .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
