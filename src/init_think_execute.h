#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"

#define BOARD_Y 6
#define BOARD_X 25

extern WINDOW *board_win;
extern WINDOW *sprites;
extern WINDOW *input_move_window;
extern WINDOW *execute_move_window;
extern board_t *board;
extern mino_t *mino;

void init_think_execute();
