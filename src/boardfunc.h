#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef struct Row row_t;

typedef enum StackOperation {
    PUSH,
    POP,
    PEEK,
    CLEAR,
} stackop_t;

typedef struct Board {
    row_t *head;
    char render_limit;
    char depth;
    char width;
    char grid[20][10]; 
    char row_counts[20];
    WINDOW *parent_window;
} board_t;

typedef struct Row {
    row_t *next;
    char data[10];
    int count;
} row_t;

void board_init(board_t *board, char depth, char width);
void board_render(board_t *board, WINDOW *window);
int row_process(board_t *board, int index, stackop_t operation);
row_t *row_at_index(board_t *board, int i);
row_t *row_iterator(row_t *head, int reset);
void board_init_sll(board_t *board, char depth, char width);
void row_clear(board_t *board, int row, int count);
int board_data_at_yx(board_t *board, int y, int x);
