#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
int num_of_strokes[] = {0, 5, 5, 5, 5, 5, 5, 5, 5,    // there is no hole 0
                        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
int current_hole = 0;
char club_type = '?'; // w = wood      | a = approach                            | p = putter
char club_sub_type = '?'; // 1, 2, 3
char club = '?'; // d = driver, 3 = 3w | 4-9 = 4i-9i, w = pw, a = aw, s = sw,| p = putter
int stage = 1;
/////// Stroke manipulation functions

static void add_stroke() {
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
static void show_current_hole() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "   Hole %u  Stroke: %u", current_hole, num_of_strokes[current_hole]);
  text_layer_set_text(text_layer, body_text);
}
static void add_and_show_total() {
  current_hole++;
  int total_score = 0;
  for (int a=1; a<19; a++) {
    total_score+=num_of_strokes[a];
  }
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "You shot: %u", total_score);
  text_layer_set_text(text_layer, body_text);
}
static void choose_club_type() {
  if (club_type == 'w') {
    text_layer_set_text(text_layer, "Driver \n---------------\n \n---------------\n 3 Wood");
  }
  else if (club_type == 'a') {
    text_layer_set_text(text_layer, "4-6 \n---------------\n 7-9 \n---------------\n P-S");
  }
  else if (club_type == 'p') {
    text_layer_set_text(text_layer, " \n---------------\n \n---------------\n Putter");
  }
}

static void choose_club_sub_type() {
  if (club_sub_type == '1') { // 1 = 4-6
    text_layer_set_text(text_layer, "4 iron \n---------------\n 5 iron \n---------------\n 6 iron");
  } 
  else if (club_sub_type == '2') { // 2 = 4-6
    text_layer_set_text(text_layer, " 7 iron \n---------------\n 8 iron \n---------------\n 9 iron");
  }
  else if (club_sub_type == '3') { // 3 = w-s
    text_layer_set_text(text_layer, " P wedge \n---------------\n A wedge \n---------------\n Sandy");
  }
}

static void choose_club() {
  if (club == 'd') {
    text_layer_set_text(text_layer, "Driver Selected");
  }
  else if (club == '3') {
    text_layer_set_text(text_layer, "3 Wood selected");
  }
  else if (club == '4') {
    text_layer_set_text(text_layer, "4 Iron selected");
  }
  else if (club == '5') {
    text_layer_set_text(text_layer, "5 Iron selected");
  }
  else if (club == '6') {
    text_layer_set_text(text_layer, "6 Iron selected");
  }
  else if (club == '7') {
    text_layer_set_text(text_layer, "7 Iron selected");
  }
  else if (club == '8') {
    text_layer_set_text(text_layer, "8 Iron selected");
  }
  else if (club == '9') {
    text_layer_set_text(text_layer, "9 Iron selected");
  }
  else if (club == 'w') {
    text_layer_set_text(text_layer, "P Wedge selected");
  }
  else if (club == 'a') {
    text_layer_set_text(text_layer, "A Wedge selected");
  }
  else if (club == 's') {
    text_layer_set_text(text_layer, "S Wedge selected");
  }
  else if (club == 'p') {
    text_layer_set_text(text_layer, "Putter selected");
  }
}
// add an undo function
/////// Click event functions

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // go back to - club type
  if (stage == 2) {
    stage = 1;
    club_type = '?';
    text_layer_set_text(text_layer, "Wood \n---------------\n Approach \n---------------\n Putter");
  }
  else if (stage == 3) {
    stage = 2;
    club = '?';
    choose_club_type();
  }
  else if (stage == 4) {
    stage = 3;
    club = '?';
    choose_club_sub_type();
  }
}
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  //... called when long click is released ...
  // do nothing
  Window *window = (Window *)context; // This context defaults to the window, but may be changed with \ref window_set_click_context.
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // choose club type
  if (stage == 1) {
    club_type = 'w';
    choose_club_type();
    stage = 2;
  }
  // choose club
  else if (stage == 2) {
    if (club_type == 'w') {
      club = 'd';
      choose_club();
      stage = 3;
    }
    else if (club_type == 'p') {
      club = 'p';
      choose_club();
      stage = 3;
    }
    else if (club_type == '1') {
      club_sub_type = '2';
      choose_club_sub_type();
      stage = 4;
    }
  }
  // choose club after sub type chosen
  else if (stage == 4) {
    if (club_sub_type == '1') {
      club = '4';
      choose_club();
    }
    else if (club_sub_type == '2') {
      club = '7';
      choose_club();
    }
    else if (club_sub_type == '1') {
      club = 'w';
      choose_club();
    }
  }
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // choose club type
  if (stage == 1) {
    club_type = 'a';
    choose_club_type();
    stage = 2;
  }
  // choose club
  else if (stage == 2) {
    if (club_type == 'w') {
      club = 'd';
      choose_club();
      stage = 3;
    }
    else if (club_type == 'p') {
      club = 'p';
      choose_club();
      stage = 3;
    }
    else if (club_type == 'a') {
      club_sub_type = '2';
      choose_club_sub_type();
      stage = 4;
    }
  }
  // choose club after sub type chosen
  else if (stage == 4) {
    if (club_sub_type == '1') {
      club = '5';
      choose_club();
    }
    else if (club_sub_type == '2') {
      club = '8';
      choose_club();
    }
    else if (club_sub_type == '1') {
      club = 'a';
      choose_club();
    }
  }
  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // choose club type
  if (stage == 1) {
    club_type = 'p';
    choose_club_type();
    stage = 2;
  }
  // choose club
  else if (stage == 2) {
    if (club_type == 'w') {
      club = '3';
      choose_club();
      stage = 3;
    }
    else if (club_type == 'p') {
      club = 'p';
      choose_club();
      stage = 3;
    }
    else if (club_type == 'a') {
      club_sub_type = '3';
      choose_club_sub_type();
      stage = 4;
    }
  }
  // choose club after sub type chosen
  else if (stage == 4) {
    if (club_sub_type == '1') {
      club = '6';
      choose_club();
    }
    else if (club_sub_type == '2') {
      club = '9';
      choose_club();
    }
    else if (club_sub_type == '1') {
      club = 's';
      choose_club();
    }
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
  text_layer_set_text(text_layer, "Wood \n---------------\n Approach \n---------------\n Putter");
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