#include <ncurses.h>
#include "boardfunc.h"

void init_board(board_t *board, char depth, char width) {
    board->depth = depth;
    board->width = width;
    board->render_limit = 19;
    for (char y = 0; y < board->depth; y++) {
        for (char x = 0; x < board->width; x++) {
            board->grid[board->depth - 1 - y][x] = 9;
        } 
    }
}

void render_board(board_t *board, WINDOW *window) {
    for (char y = board->render_limit; y < board->depth; y++) {
        for (char x = 0; x < board->width; x++) {
            char col = board->grid[board->render_limit + board->depth - 1 - y][x];
            if (col != 9) {
                wattron(window, COLOR_PAIR(col));
                mvwaddch(window, board->render_limit + (board->depth - 1 - y), x * 2, 'x'); 
                waddch(window, 'x'); 
            }
        } 
    }
}
