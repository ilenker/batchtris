#include <ncurses.h>
#include "boardfunc.h"

void init_board(board_t *board, char depth, char width) {
    board->depth = depth;
    board->width = width;
    board->render_limit = 19;
    for (int y = 0; y < board->depth; y++) {
        board->row_counts[y] = 0;
        for (int x = 0; x < board->width; x++) {
            board->grid[board->depth - 1 - y][x] = 9;
        } 
    }
}

void render_board(board_t *board, WINDOW *window) {
    for (int y = board->render_limit; y < board->depth; y++) {
        for (int x = 0; x < board->width; x++) {
            char col = board->grid[board->render_limit + board->depth - 1 - y][x];
            if (col != 9) {
                wattron(window, COLOR_PAIR(col));
                mvwaddch(window, board->render_limit + (board->depth - 1 - y), x * 2, 'x'); 
                waddch(window, 'x'); 
            }
        } 
    }
}

int process_rows(board_t *board, int row, stackop_t operation) {
    static int stack[16];  
    static int ptr = 0;

    switch (operation) {
        case PUSH:
            stack[ptr] = row;
            ptr++;
            return ptr;
            break;
        case POP:
            return ptr;
            break;
        case CLEAR:
            if (ptr != 0) {
                while (ptr >= 0) { 
                    board->row_counts[stack[ptr]] = 0;
                    stack[ptr] = -1;
                    ptr--; 
                }
                ptr = 0;
            }
            return ptr;
            break;
    }
}
