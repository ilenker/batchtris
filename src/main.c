#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "boardfunc.h"
#include "menus.h"
#include "sprites.h"
#include "init_think_execute.h"
#include "gameloops.h"

int main() {
    time_t t;
    srand(time(&t));
    init_think_execute();
    menus_init();

    sprites_minos_init(sprites, &sprite_data[0]);

    //sequence_t *first_seq = calloc(1, sizeof(sequence_t));
    //event_t ev1 = {event_message, ui_make_message, "message one letsgoo", 8, 50, 0};
    //event_t ev2 = {event_message, ui_make_message, "anotha one", 11, 48, 0};
    //event_t evnull = {event_null, NULL, NULL, 0, 0, 0};
    //for (int i = 0; i < 32; i++) {
    //    first_seq->frame[i] = &evnull;
    //}
    //first_seq->frame[0] = &ev1;
    //first_seq->frame[20] = &ev2;
    //first_seq->active = false;
    //int timer = 0;

    game_variables_init();

_menu_: ;
    while (GAME_STATE == MENU) {
        GAME_STATE = mode_select();
    }

_classic_tetris_: ;
    refresh();
    doupdate();
    while (GAME_STATE == CLASSIC) {
        GAME_STATE = classic_tetris();
    }

     // UI Setup 
    if (GAME_STATE == THINK) {
        mvprintw(BOARD_Y + 21, BOARD_X, "1-2-3-4-5-6-7-8-9 10");
        mvprintw(BOARD_Y + 22, BOARD_X, "      ╰─home─╯      ");
        nodelay(board_win, false);
        nodelay(stdscr, false);
        mvprintw(BOARD_Y - 2, BOARD_X + 1, "!think! ");
        wnoutrefresh(board_win);
        wnoutrefresh(input_move_window);
        doupdate();
        werase(input_move_window);
        mvprintw(BOARD_Y + 4,  BOARD_X + 20, "├────────╮");
        mvprintw(BOARD_Y + 7,  BOARD_X + 20, "├────────╯");
    }

_think_: ;
    bag_q_render(0, 4);
    wattron(input_move_window, COLOR_PAIR(board->bag[board->bag_index] + 16));
    while (GAME_STATE == THINK) {
        GAME_STATE = input_moves();
    }
    werase(execute_move_window);
    while (GAME_STATE == EXECUTE) {
        GAME_STATE = execute_moves();
    }

    switch (GAME_STATE) {
        case THINK:
            goto _think_;
        case MENU:
            goto _menu_;
        case CLASSIC:
            goto _classic_tetris_;
        case RESULTS:
            break;
        case EXECUTE:
            break;
        case QUIT:
            break;
    }
}
