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
extern board_t *board;
extern mino_t *mino;
extern int BOARD_Y;
extern int BOARD_X;


void init_think_execute();
