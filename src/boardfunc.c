#include <ncurses.h>
#include "boardfunc.h"

void init_board(board_t *board, char depth, char width) {
    board->depth = depth;
    board->width = width;
    board->render_limit = board->depth;
    for (char y = 0; y < board->depth; y++) {
        for (char x = 0; x < board->width; x++) {
            board->grid[board->depth - 1 - y][x] = 9;
        } 
    }
    board->grid[19][0] = 1;
    board->grid[19][1] = 1;
    board->grid[19][2] = 1;
    board->grid[19][3] = 1;
}

void render_board(board_t *board, WINDOW *window) {
    for (char y = 0; y < board->depth; y++) {
        for (char x = 0; x < board->width; x++) {
            wattron(window, COLOR_PAIR(board->grid[board->depth - 1 - y][x]));
            mvwaddch(window, (board->depth - 1 - y), x * 2, '*'); 
            waddch(window, '*'); 
        } 
    }
}
