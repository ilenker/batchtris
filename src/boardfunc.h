#pragma once

#include <ncurses.h>

#define QUEUE_PREVIEW_LENGTH 5

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
    int bag[14];
    int bag_index;
} board_t;

typedef struct Row {
    row_t *next;
    char data[10];
    int count;
} row_t;

void board_init(char depth, char width);
void board_render();
int row_process(int index, stackop_t operation);
row_t *row_at_index(int i);
row_t *row_iterator(row_t *head, int reset);
void board_init_sll(bool rows_free);
void row_clear(int row, int count);
int board_data_at_yx(int y, int x);
int bag_next();
void bag_shuffle(int from_index);
