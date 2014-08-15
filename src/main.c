#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
int num_of_strokes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num_of_putts[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool num_of_GIR[] = {false,false,false,false,false,false,false,false,false,false,
                     false,false,false,false,false,false,false,false,false,false};
char num_of_FIR[] = {'-', '-', '-', '-', '-', '-', 
                     '-', '-', '-', '-', '-', '-', 
                     '-', '-', '-', '-', '-', '-'};
int num_of_possible_FIR = 0;

int current_hole = 1; // initialize to first hole

char clubs_selected[19][11] = {  
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'},
                          {'-','-','-','-','-','-','-','-','-','-','-'}
                          };



// pre round:
bool round_start = false;
bool round_type_selected = false;
bool pre_round_summary_shown = false;

bool next_shot_is_tee_shot = true; // assigned value of true after a putt is made
bool round_complete = false;
bool tee_shot_result_pending = false;

int par_for_each_hole[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int pre_round_hole_iterator = 1;
int holes_in_round = 18;

////

struct golf_course {
  int par_for_each_hole[20];
  int holes_in_round;
  char name[25];
};

struct golf_course bantys_roost = {{3,4,5,3,4,5,3,4,5,3,4,5,3,4,5,3,4,5}, 18, "Banty's Roost"};

// course selection
/*static void select_golf_course() {
  par_for_each_hole
}*/

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
static void show_round_type() {
  static char body_text[50];
  // show round type choices
  snprintf(body_text, sizeof(body_text), "18 holes\n---------\n9 holes\n---------\n");
  text_layer_set_text(text_layer, body_text);
}

static void show_par_for_each_hole() {
  static char body_text[50];
  // show round type choices
  snprintf(body_text, sizeof(body_text), "Par 3\n---H %u----\nPar 4\n---------\nPar 5",pre_round_hole_iterator);
  text_layer_set_text(text_layer, body_text);
}

static void show_pre_round_summary() {
  int course_par = 0;
  for (int a=1; a < holes_in_round+1; a++) { //  add up course par
    // get number of FIR's possible
    if (par_for_each_hole[a] > 3) { // par 4's and 5's
      num_of_possible_FIR++;
    }
    
    course_par+=par_for_each_hole[a];
  }
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "Is this right?\n%u Holes\nPar %u\nPress any key", holes_in_round, course_par);
  text_layer_set_text(text_layer, body_text);
}

static void add_and_show_total() { // add up strokes and putts and show them
  int total_score = 0;
  int total_putts = 0;
  int total_GIR = 0;
  int total_FIR = 0;
  int total_tee_left = 0;
  int total_tee_right = 0;
  // Fairway in regulation
  for (int a=1; a<holes_in_round+1; a++) {
    if (num_of_FIR[a] == 'f') { // if FIR is true
      total_FIR++;
    }
    else if (num_of_FIR[a] == 'l') {
      total_tee_left++;
    }
    else if (num_of_FIR[a] == 'r') {
      total_tee_right++;
    }
  }
  // Green in regulation
  for (int a=1; a<holes_in_round+1; a++) {
    if (num_of_strokes[a]-num_of_putts[a] <= par_for_each_hole[a]) { // if GIR is true
      num_of_GIR[a] = true;
    }
  }
  for (int a=1; a<holes_in_round+1; a++) {
    total_score+=num_of_strokes[a];
    total_putts+=num_of_putts[a];
    total_GIR+=num_of_GIR[a];
  }
  
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "You shot: %u\n%u Putts\n%u/%u GIR\n%u < %u/%u FIR > %u", total_score, total_putts, total_GIR, holes_in_round, total_tee_left, total_FIR, num_of_possible_FIR, total_tee_right);
  text_layer_set_text(text_layer, body_text);
  
  round_complete = true;
}

static void show_tee_shot_result() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  <--  S%u\n%c%c%c%c%c%c%c%c%c%c\nFIR\n ---------  \n  -->  ", 
          current_hole, num_of_strokes[current_hole],
          clubs_selected[current_hole][1], clubs_selected[current_hole][2],
          clubs_selected[current_hole][3], clubs_selected[current_hole][4],
          clubs_selected[current_hole][5], clubs_selected[current_hole][6],
          clubs_selected[current_hole][7], clubs_selected[current_hole][8],
          clubs_selected[current_hole][9], clubs_selected[current_hole][10]);
  text_layer_set_text(text_layer, body_text);
}

static void show_club_selection() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  Drive  S%u\n%c%c%c%c%c%c%c%c%c%c\nApproach\n--Par %u--\nPutt", 
          current_hole, num_of_strokes[current_hole],
          clubs_selected[current_hole][1], clubs_selected[current_hole][2],
          clubs_selected[current_hole][3], clubs_selected[current_hole][4],
          clubs_selected[current_hole][5], clubs_selected[current_hole][6],
          clubs_selected[current_hole][7], clubs_selected[current_hole][8],
          clubs_selected[current_hole][9], clubs_selected[current_hole][10], par_for_each_hole[current_hole]);
  text_layer_set_text(text_layer, body_text);
}

static void show_club_selection_at_18th() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "H%u  Finish  S%u\n%c%c%c%c%c%c%c%c%c%c\nRound\n--Par %u--\nPutt", 
          current_hole, num_of_strokes[current_hole],
          clubs_selected[current_hole][1], clubs_selected[current_hole][2],
          clubs_selected[current_hole][3], clubs_selected[current_hole][4],
          clubs_selected[current_hole][5], clubs_selected[current_hole][6],
          clubs_selected[current_hole][7], clubs_selected[current_hole][8],
          clubs_selected[current_hole][9], clubs_selected[current_hole][10], par_for_each_hole[current_hole]);
  text_layer_set_text(text_layer, body_text);
}


/////// Click event functions
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) { // undo last hole
  if (round_start == true) {
    if (num_of_strokes[current_hole] == 0) { // current hole is already 0
      prev_hole();
    }
    else { // current hole still has strokes in it
      for (int a = 0; a<12;a++) {
        clubs_selected[current_hole][a] = '-';
      }
      num_of_strokes[current_hole] = 0;
    }
    round_complete = false;
    next_shot_is_tee_shot = false;
    show_club_selection();
  }
}
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  //... called when long click is released ...
  // do nothing
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  /////// choose round length
  if (round_start == false && round_type_selected == false) {
    // choose 9 or 18 holes
    holes_in_round = 18;
    round_type_selected = true;
    show_par_for_each_hole();
  }
  ////// choose par for each hole
  else if (round_start == false && round_type_selected == true) {
    if (pre_round_summary_shown == true) {
      round_start = true;
      show_club_selection();
    }
    else if (pre_round_hole_iterator == holes_in_round) {
      par_for_each_hole[pre_round_hole_iterator] = 3;
      show_pre_round_summary();
      pre_round_summary_shown = true;
    }
    else {
      show_par_for_each_hole();
    }
    par_for_each_hole[pre_round_hole_iterator] = 3;
    pre_round_hole_iterator++;
  }
  ////// enter each shot
  else if (round_start == true && round_complete == false) {
    // select the tee shot result (after a tee shot from a par 4-5 is made)
    if (tee_shot_result_pending == true) {
      num_of_FIR[current_hole] = 'l';     
      show_club_selection();
      tee_shot_result_pending = false;
    }
    // if select is pressed after putting out the hole -> tee shot
    else if (next_shot_is_tee_shot == true) { 
      // select is pressed after putting out the final hole =
      if (current_hole == holes_in_round) {
        add_and_show_total();
      }
      // a tee shot on a par 4-5 = 
      else if (par_for_each_hole[current_hole] > 3) { // tee shot on par 4 -5
        tee_shot_result_pending = true;
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('d');
        clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
        show_tee_shot_result();
      }
      // tee shot on par 3
      else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('d');
        clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
        show_club_selection();
      }
    }
    // not a tee shot = stay in the same hole if there are no putts yet in this hole
    else {
      add_stroke('d');
      clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
      show_club_selection();
    }
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_start == false && round_type_selected == false) {
    // choose 9 or 18 holes
    holes_in_round = 9;
    round_type_selected = true;
    show_par_for_each_hole();
  }
  else if (round_start == false && round_type_selected == true) {
    // choose each par
    if (pre_round_summary_shown == true) {
      round_start = true;
      show_club_selection();
    }
    else if (pre_round_hole_iterator == holes_in_round) { // done round
      par_for_each_hole[pre_round_hole_iterator] = 4;
      show_pre_round_summary();
      pre_round_summary_shown = true;
    }
    else {
      show_par_for_each_hole();
    }
    par_for_each_hole[pre_round_hole_iterator] = 4;
    pre_round_hole_iterator++;
  }
  else if (round_start == true && round_complete == false) {
    if (tee_shot_result_pending == true) {
      num_of_FIR[current_hole] = 'f';     
      show_club_selection();
      tee_shot_result_pending = false;
    }
    else if (next_shot_is_tee_shot == true) { // proceed to next hole if last stroke was a putt
      if (current_hole == holes_in_round) { // if select is pressed after putting out the final hole
        add_and_show_total();
      }
      else if (par_for_each_hole[current_hole] > 3) { // tee shot on par 4-5
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('a');
        clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
        show_tee_shot_result();
        tee_shot_result_pending = true;
      }
      else { // tee shot on par 3
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('a');
        clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
        show_club_selection();
      }
    }
    else { // stay in the same hole if there are no putts yet in this hole
      add_stroke('a');
      clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
      show_club_selection();
    }
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (round_start == false && round_type_selected == false) {
    // choose 9 or 18 holes - chose a blank
    show_round_type();
  }
  else if (round_start == false && round_type_selected == true) {
    // choose each par
    if (pre_round_summary_shown == true) {
      round_start = true;
      show_club_selection();
    }
    else if (pre_round_hole_iterator == holes_in_round) { // done round
      par_for_each_hole[pre_round_hole_iterator] = 5;
      show_pre_round_summary();
      pre_round_summary_shown = true;
    }
    else {
      show_par_for_each_hole();
    }
    par_for_each_hole[pre_round_hole_iterator] = 5;
    pre_round_hole_iterator++;
  }
  else if (round_complete == false) {
    if (tee_shot_result_pending == true) { 
      num_of_FIR[current_hole] = 'r';     
      show_club_selection();
      tee_shot_result_pending = false;
    }
    else if (current_hole == holes_in_round) {
      add_stroke('p');
      clubs_selected[current_hole][num_of_strokes[current_hole]] = 'p';
      show_club_selection_at_18th();
      next_shot_is_tee_shot = true;
    }
    else {
      add_stroke('p');
      clubs_selected[current_hole][num_of_strokes[current_hole]] = 'p';
      show_club_selection();
      next_shot_is_tee_shot = true;
    }
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
  snprintf(body_text, sizeof(body_text), "18 holes\n---------\n9 holes\n---------\n");
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
