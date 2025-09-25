#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef struct Board {
    char render_limit;
    char depth;
    char width;
    char grid[20][10]; 
} board_t;

void init_board(board_t *board, char depth, char width);
void render_board(board_t *board, WINDOW *window);
