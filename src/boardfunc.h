#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef enum StackOperation {
    PUSH,
    POP,
    CLEAR,
} stackop_t;

typedef struct Board {
    char render_limit;
    char depth;
    char width;
    char grid[20][10]; 
    char row_counts[20];
} board_t;

void init_board(board_t *board, char depth, char width);
void render_board(board_t *board, WINDOW *window);
int process_rows(board_t *board, int index, stackop_t operation);
