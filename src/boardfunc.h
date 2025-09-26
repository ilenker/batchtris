#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef struct Row row_t;

typedef enum StackOperation {
    PUSH,
    POP,
    CLEAR,
} stackop_t;

typedef struct Board {
    char render_limit;
    char depth;
    char width;
    row_t *head;
    char grid[20][10]; 
    char row_counts[20];
} board_t;

typedef struct Row {
    char data[10];
    row_t *next;
} row_t;

void init_board(board_t *board, char depth, char width);
void render_board(board_t *board, WINDOW *window);
int process_rows(board_t *board, int index, stackop_t operation);
row_t *row_at_index(board_t *board, int i);
row_t *yield_next_row(row_t *head, int reset);
void init_board_sll(board_t *board, char depth, char width);
void clear_rows(board_t *board, int row, int count);
int get_board_data_yx(board_t *board, int y, int x);
