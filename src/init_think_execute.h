#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"

extern WINDOW *board_win;
extern WINDOW *sprites;
extern WINDOW *input_move_window;
extern WINDOW *execute_move_window;
extern WINDOW *debug_window;
extern WINDOW *main_win;
extern WINDOW *options_win;
extern WINDOW *controls_win;
extern WINDOW *tooltip_win;
extern WINDOW *ttbox_win;
extern mino_t *mino;
extern board_t *board;
extern int BOARD_Y;
extern int BOARD_X;


void think_execute_init();
void display_reset();
void colors_init(bool standard);
