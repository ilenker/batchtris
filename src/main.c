#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>

#include "boardfunc.h"
#include "sprites.h"
#include "init_think_execute.h"
#include "gameloops.h"

float lerp(float a, float b, float f) {
    return (a * (1.0 - f)) + (b * f);
}

int main() {
    time_t t;
    srand(time(&t));
    init_think_execute();

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

    refresh();
    doupdate();

    float start, end, f, df, ddf;
    start = 10.0f;
    end = 30.0f;
    f = 0.01f;
    df = 0.005f;
    ddf = 1.01f;

    while (true) {
        erase();
        mvaddstr(10, lerp(start, end, f), "x");
        mvprintw(12, 20, "%f", f);
        df *= ddf;
        f += df; 
        if (f < 0.51f) ddf = 1.10f;
        if (f > 0.49f) ddf = 0.90f;
        wnoutrefresh(stdscr);
        doupdate();
        napms(17);
    }

    //_start_: ;
    //df = 1.08f;
    //while (lerp(start, end, f) <= end) {
    //    erase();
    //    mvaddstr(lerp(start, end, f), 10, "x");
    //    f *= df; 
    //    df += df/200; 
    //    wnoutrefresh(stdscr);
    //    doupdate();
    //    napms(17);
    //}
    //df = 0.92f;
    //while (lerp(start, end, f) >= start + 1.5f) {
    //    erase();
    //    mvaddstr(lerp(start, end, f), 10, "x");
    //    f *= df; 
    //    df -= df/200;
    //    wnoutrefresh(stdscr);
    //    doupdate();
    //    napms(17);
    //}
    //goto _start_ ;

    _menu_: ;
    nodelay(stdscr, 0);
    while (GAME_STATE == MENU) {
        GAME_STATE = mode_select();
    }
    nodelay(stdscr, 1);

    refresh();
    doupdate();

    while (GAME_STATE == CLASSIC_TETRIS) {
        classic_tetris();
    }

    if (GAME_STATE == MENU) {
        goto _menu_;
    }

    _inputmoves_: ; 
    mvprintw(BOARD_Y + 20, BOARD_X, "L_L_L_L_^^L_L_L_L_L_");
    nodelay(board_win, false);
    nodelay(stdscr, false);
    mvprintw(BOARD_Y - 1, BOARD_X + 6, " !think! ");
    wnoutrefresh(board_win);
    wnoutrefresh(input_move_window);
    doupdate();
    mvprintw(BOARD_Y + 4,  BOARD_X + 22, "  \\/");

    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 0) % 14]], BOARD_Y + 5,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 8,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 11, BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 14, BOARD_X + 21);

    while (GAME_STATE == INPUT_MOVES) {
        input_moves();
    }

    werase(execute_move_window);

    while (GAME_STATE == EXECUTE_MOVES) {
        execute_moves();
    }
    goto _inputmoves_;
}
