#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
int num_of_strokes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0,    // there is no hole 0
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int current_hole = 1;
bool round_complete = false;

int num_of_putts[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

bool next_shot_is_tee_shot = false;

char clubs_selected[200];
char club_type = '?'; // w = wood      | a = approach                            | p = putter
char club_sub_type = '?'; // 1, 2, 3
char club = '?'; // d = driver, 3 = 3w | 4-9 = 4i-9i, w = pw, a = aw, s = sw,| p = putter
int stage = 1;
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
  if (current_hole == 19) {
    current_hole = 1;
  }
  else {
    current_hole++;
  }
}
static void prev_hole() {
  if (current_hole == 0) {
    current_hole = 18;
  }
  else {
    current_hole--;
  }
}
/////// update window

static void add_and_show_total() {
  current_hole++;
  int total_score = 0;
  int total_putts = 0;
  for (int a=1; a<19; a++) {
    total_score+=num_of_strokes[a];
    total_putts+=num_of_putts[a];
  }
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "You shot: %u\n%uPutts", total_score, total_putts);
  text_layer_set_text(text_layer, body_text);
  
  round_complete = true;
}
static void show_club_selection() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  Drive  S%u\n----------\nApproach\n----------\nPutt", current_hole, num_of_strokes[current_hole]);
  text_layer_set_text(text_layer, body_text);
}

static void main_menu() {
  text_layer_set_text(text_layer, "<-- \n---------------\n FIR \n---------------\n -->");
  text_layer_set_text(text_layer, "<-- \n---------------\n Center \n---------------\n -->");
}
  
/////// Click event functions

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // undo last hole
  next_shot_is_tee_shot = true;
  round_complete = false;
  prev_hole();
  num_of_strokes[current_hole] = 0;
  show_club_selection();

}
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  //... called when long click is released ...
  // do nothing
  Window *window = (Window *)context; // This context defaults to the window, but may be changed with \ref window_set_click_context.
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    //club_selected[current_stroke] = 'd';
    if (next_shot_is_tee_shot == true) { // go to next next if last stroke was a putt
      if (current_hole == 18) {
        add_and_show_total();
      }
      else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('d');
        show_club_selection();
      }
    }
    else {
      add_stroke('d');
      show_club_selection();
    }
  }


  else {
    // show more stats or somethign
  }
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    //club_selected[current_stroke] = 'd';
    if (next_shot_is_tee_shot == true) { // go to next next if last stroke was a putt
      if (current_hole == 18) {
        add_and_show_total();
      }
      else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('a');
        show_club_selection();
      }
    }
    else {
      add_stroke('a');
      show_club_selection();
    }
  }

  else {
    // show more stats or somethign
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_complete == false) {
    add_stroke('p');
    show_club_selection();
    next_shot_is_tee_shot = true;
  }

  else {
    // show more stats or somethign
  }
}

/////////// configuration

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
   // long click config:
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
