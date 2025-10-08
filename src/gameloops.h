#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>

#include "boardfunc.h"
#include "minofunc.h"
#include "sprites.h"

#define SUCCESS_NOUPDATE 0
#define SUCCESS_UPDATE   2
#define FAIL_NOUPDATE    3
#define BETS_ARE_OFF    -1

typedef void (*event_function_t)(WINDOW *window, char *msg, int y, int x);

typedef enum GlobalGameState {
    MENU,
    THINK,
    EXECUTE,
    CLASSIC,
    RESULTS,
    QUIT,
} game_state_t;


typedef enum EventName {
    event_null = 0,
    event_xion_blinds = 1,
    event_xion_fade = 2,
    event_wipe = 3,
    event_message = 4,
    event_stats = 5,
    event_store_window = 20,
    event_recall_window = 21,
    event_pause = 32,
    event_resume = 33,
    event_next_sequence = 69,
} eventname_t;


typedef struct Event {
    eventname_t type;
    event_function_t event;
    void *content;
    int y;
    int x;
    int length;
} event_t;

typedef struct Stats {
    unsigned int pc;
    float kpp;
    float pps;
} stats_t;

void ui_make_message(WINDOW *window, char *msg, int y, int x);
game_state_t classic_tetris();
game_state_t input_moves();
game_state_t execute_moves();
void game_variables_init();
void edit_board();
void bag_cursor_render(int *bag_cursor); 
void bag_q_render(int from_idx, int queue_len);
void all_delay_set(bool state);
char input_remap(char unmapped_input);

typedef struct Sequence {
    event_t *frame[32];
    bool active;
} sequence_t;

// general init 
extern char input;
extern int state_update;
extern row_t *row_iterator_index;
extern bool hold_available;
extern game_state_t GAME_STATE;
extern game_state_t PREV_GAME_STATE;
extern shape_t hold;
extern shape_t input_phase_hold;
extern char g_debug_verbosity;
extern int g_gravity_timer;
extern stats_t stats;

extern sprite_t sprite_data[16];

// input loop stuff 
extern game_input_t input_sequence[64];
extern int seq_index; 
extern int seq_len; 
extern int piece_count;
extern char *mn[9];
