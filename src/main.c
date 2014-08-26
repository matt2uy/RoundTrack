#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
int num_of_strokes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num_of_putts[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int num_of_penalties[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool num_of_GIR[] = {false,false,false,false,false,false,false,false,false,false,
                     false,false,false,false,false,false,false,false,false,false};
char num_of_FIR[] = {'-', '-', '-', '-', '-', '-', 
                     '-', '-', '-', '-', '-', '-', 
                     '-', '-', '-', '-', '-', '-'};

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



// program sequence
bool course_selected = false;
bool round_start = false;
bool next_shot_is_tee_shot = true; // assigned value of true after a putt is made
bool round_complete = false;
bool tee_shot_result_pending = false;
bool first_hole_tee_shot_done = false;// messy

// duntroon
char name[25] = "Duntroon";
int hole_yardage[30] = {0, 490, 330, 102, 319, 320, 346, 317, 156, 415,   // there is no hole 0
                      452, 311, 288, 158, 207, 502, 335, 406, 442};
int par_for_each_hole[30] = {0, 5, 4, 3, 4, 4, 4, 4, 3, 4,   // there is no hole 0
                      5, 4, 4, 3, 3, 5, 4, 4, 4};
int holes_in_round = 18;
int total_par = 71;
int num_of_possible_FIR = 14;

///////


////

struct golf_course {
  char name[25];
  int hole_yardage[20];
  int holes_in_round;
  int par_for_each_hole[20];
  int total_par;
  int num_of_possible_FIR;
};

struct golf_course duntroon_highlands = {"Duntroon Highlands", // name
    {0, 490, 330, 102, 319, 320, 346, 317, 156, 415, 452, 311, 288, 158, 207, 502, 335, 406, 442}, // hole yardages
    18, // num of holes
    {0, 5, 4, 3, 4, 4, 4, 4, 3, 4, 5, 4, 4, 3, 3, 5, 4, 4, 4}, // hole pars
    71, // total par
    14// num of possible FIR
    };
struct golf_course bantys_roost = {"Banty's Roost", // name
    {0, 490, 330, 102, 319, 320, 346, 317, 156, 415, 452, 311, 288, 158, 207, 502, 335, 406, 442}, // hole yardages
    18, // num of holes
    {0, 5, 4, 3, 4, 4, 4, 4, 3, 4, 5, 4, 4, 3, 3, 5, 4, 4, 4}, // hole pars
    71, // total par
    14// num of possible FIR
    };
struct golf_course glen_eagle = {"Glen Eagle", // name
    {0, 490, 330, 102, 319, 320, 346, 317, 156, 415, 452, 311, 288, 158, 207, 502, 335, 406, 442}, // hole yardages
    18, // num of holes
    {0, 5, 4, 3, 4, 4, 4, 4, 3, 4, 5, 4, 4, 3, 3, 5, 4, 4, 4}, // hole pars
    71, // total par
    14// num of possible FIR
    };

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

static void set_up_course() {
  holes_in_round = duntroon_highlands.holes_in_round;
  total_par = duntroon_highlands.total_par;
  //name = bantys_roost.name;
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "%s\n%u Holes\nPar %u\nPress any key", name, holes_in_round, total_par);
  text_layer_set_text(text_layer, body_text);
}

static void show_post_round_summary() { // add up strokes and putts and show them
  int total_score = 0;
  int total_putts = 0;
  int total_GIR = 0;
  int total_FIR = 0;
  int total_tee_left = 0;
  int total_tee_right = 0;
  int total_penalties = 0;
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
    total_penalties+=num_of_penalties[a];
  }
  
  static char body_text[100];
  snprintf(body_text, sizeof(body_text), "%u Strokes\n%u Putts\n%u/%u GIR\n%u < %u/%u FIR > %u\n %u Penalties", total_score, total_putts, total_GIR, holes_in_round, total_tee_left, total_FIR, num_of_possible_FIR, total_tee_right, total_penalties);
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
// hold up click = 1 stroke penalty
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) { // undo last hole
  if (round_start == true) {
    num_of_penalties[current_hole]++;      
    add_stroke('x');
    clubs_selected[current_hole][num_of_strokes[current_hole]] = 'x';
    show_club_selection();
    next_shot_is_tee_shot = false;
  }
}
static void up_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {}

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
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  /////// choose round length
  if (round_start == false && course_selected == false) {
    // choose 9 or 18 holes
    holes_in_round = 18;
    set_up_course();
    course_selected = true;
  }
  else if (round_start == false && course_selected == true) {
    show_club_selection();
    round_start = true;
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
        show_post_round_summary();
      }
      // a tee shot on a par 4-5 = 
      else if (par_for_each_hole[current_hole] > 3) { // tee shot on par 4 -5
        // temporary special case for hole 1
        if (current_hole == 1 && first_hole_tee_shot_done == false) {
          first_hole_tee_shot_done = true;
          tee_shot_result_pending = true;
          next_shot_is_tee_shot = false;
          add_stroke('d');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
          show_tee_shot_result();
        }
        else {
          tee_shot_result_pending = true;
          next_shot_is_tee_shot = false;
          next_hole();
          add_stroke('d');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
          show_tee_shot_result();
        }
      }
      // tee shot on par 3
      else if (par_for_each_hole[current_hole] == 3){
        // temporary special case for hole 1
        if (current_hole == 1 && first_hole_tee_shot_done == false) {
          next_shot_is_tee_shot = false;
          add_stroke('d');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
          show_club_selection();
        }
        else {
          next_shot_is_tee_shot = false;
          next_hole();
          add_stroke('d');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'd';
          show_club_selection();
        }
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
  /////// choose round length
  if (round_start == false && course_selected == false) {
    // choose 9 or 18 holes
    holes_in_round = 9;
    course_selected = true;
    set_up_course();
  }
  else if (round_start == false && course_selected == true) {
    show_club_selection();
    round_start = true;
  }
  else if (round_start == true && round_complete == false) {
    if (tee_shot_result_pending == true) {
      num_of_FIR[current_hole] = 'f';     
      show_club_selection();
      tee_shot_result_pending = false;
    }
    else if (next_shot_is_tee_shot == true) { // proceed to next hole if last stroke was a putt
      if (current_hole == holes_in_round) { // if select is pressed after putting out the final hole
        show_post_round_summary();
      }
      else if (par_for_each_hole[current_hole] > 3) { // tee shot on par 4-5
        // temporary special case for hole 1
        if (current_hole == 1 && first_hole_tee_shot_done == false) {
          first_hole_tee_shot_done = true;
          next_shot_is_tee_shot = false;
          add_stroke('a');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
          show_tee_shot_result();
          tee_shot_result_pending = true;
        }
        else {
          next_shot_is_tee_shot = false;
          next_hole();
          add_stroke('a');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
          show_tee_shot_result();
          tee_shot_result_pending = true;
        }
      }
      else if (par_for_each_hole[current_hole] == 3) { // tee shot on par 3
        // temporary special case for hole 1
        if (current_hole == 1 && first_hole_tee_shot_done == false) {
          next_shot_is_tee_shot = false;
          add_stroke('a');
          clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
          show_club_selection();
        }
        else {
        next_shot_is_tee_shot = false;
        next_hole();
        add_stroke('a');
        clubs_selected[current_hole][num_of_strokes[current_hole]] = 'a';
        show_club_selection();
        }
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
  /////// choose round length
  if (round_start == false && course_selected == false) {
    // choose 9 or 18 holes
    holes_in_round = 18;
    course_selected = true;
    set_up_course();
  }
  else if (round_start == false && course_selected == true) {
    show_club_selection();
    round_start = true;
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
  window_long_click_subscribe(BUTTON_ID_UP, 700, up_long_click_handler, up_long_click_release_handler);
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