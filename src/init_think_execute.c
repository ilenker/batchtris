#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include "boardfunc.h"
#include "minofunc.h"
#include "sprites.h"
#include "init_think_execute.h"

#define COLOR_ORANGE     9
#define COLOR_PURPLE    10
#define BB               9
#define WB               8

WINDOW *board_win;
WINDOW *sprites;
WINDOW *input_move_window;
WINDOW *execute_move_window;
WINDOW *debug_window;
WINDOW *menu_main;
WINDOW *menu_options;
WINDOW *menu_controls;
board_t *board;
mino_t *mino;
int BOARD_Y;
int BOARD_X;


void init_think_execute() {
    //___________INIT___________//
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, 0);
    nodelay(stdscr, 1);
    leaveok(stdscr, 1);
    curs_set(0);

    // TODO: support 8 color terminals
    start_color();
    init_color(COLOR_WHITE, 929, 808, 905);
    init_color(COLOR_ORANGE, 929, 500, 100);
    init_color(COLOR_PURPLE, 650, 20, 900);
    init_color(COLOR_BLUE, 250, 320, 810);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);            // I
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);       // O 
    init_pair(3, COLOR_BLUE, COLOR_BLUE);          // J
    init_pair(4, COLOR_ORANGE, COLOR_ORANGE);     // L
    init_pair(5, COLOR_GREEN, COLOR_GREEN);      // S
    init_pair(6, COLOR_RED, COLOR_RED);         // Z
    init_pair(7, COLOR_PURPLE, COLOR_PURPLE);  // T
    init_pair(8, COLOR_WHITE, COLOR_BLACK);  // Text 
    init_pair(9, COLOR_BLACK, COLOR_BLACK);   // Blank 
    init_pair(10, COLOR_WHITE, COLOR_BLACK); // Text 
        // Mino colours for text
    init_pair(17, COLOR_CYAN, COLOR_BLACK);          // I
    init_pair(18, COLOR_YELLOW, COLOR_BLACK);       // O 
    init_pair(19, COLOR_BLUE, COLOR_BLACK);        // J
    init_pair(20, COLOR_ORANGE, COLOR_BLACK);     // L
    init_pair(21, COLOR_GREEN, COLOR_BLACK);     // S
    init_pair(22, COLOR_RED, COLOR_BLACK);      // Z
    init_pair(23, COLOR_PURPLE, COLOR_BLACK);  // T

           /* BOARD WINDOW */
    BOARD_Y = LINES / 2 - 10;
    BOARD_X = COLS / 2 - 10;
    board_win = newwin(20, 20, BOARD_Y, BOARD_X); // 20x20 board at y=6, x=6  
    scrollok(board_win, 0);
    leaveok(board_win, 1);
    nodelay(board_win, 1);
    wbkgd(board_win, COLOR_PAIR(BB) | '9');
    refresh();

    board = calloc(1, sizeof(board_t));
    board->parent_window = board_win;
    board_init_sll(false);

    row_t *current_row;
    row_iterator(NULL, 1);
    for (int y = 0; y < 20; y++) {
        current_row = row_iterator(board->head, 0); 
        if (current_row == NULL) {
            break;
        } 
        current_row->count = 0;
    }
    row_iterator(NULL, 1);  
    board_init(20, 10);

            /* MINO */
    mino = mino_init(board->bag[0]);

         /* SPRITE PAD */
    sprites = newpad(16, 16); 

         /* THINK EXECUTE WINDOWS */
    input_move_window = newwin(10, 9, BOARD_Y + 5, BOARD_X - 11); 
    wattron(input_move_window, COLOR_PAIR(WB));
    scrollok(input_move_window , 0);
    leaveok(input_move_window , 1);
    nodelay(input_move_window , 0);
    execute_move_window = newwin(10, 15, BOARD_Y + 5, BOARD_X + 30);
    wattron(execute_move_window, COLOR_PAIR(WB));
    scrollok(execute_move_window , 0);
    leaveok(execute_move_window , 1);
    nodelay(execute_move_window , 0);

    debug_window = newwin(10, 20, BOARD_Y + board->depth + 1, BOARD_X);
    wattron(debug_window, COLOR_PAIR(WB));
    scrollok(debug_window, 0);
    leaveok(debug_window, 1);
    nodelay(debug_window, 0);



            /* UI BORDERS */
    //╭────────╮
    //╰────────╯                       // 0123456789abcdefghijkl
    mvaddstr(BOARD_Y -  3, BOARD_X + 0,     "╭─────────╮");
    mvaddstr(BOARD_Y -  2, BOARD_X + 0,     "│         │");
    mvaddstr(BOARD_Y -  1, BOARD_X - 1,    "╭┴─────────┴─────────╮");
    for (int i = 0; i < 20; i++)
        mvaddstr(BOARD_Y + i, BOARD_X - 1, "│                    │");

    mvaddstr(BOARD_Y + 20, BOARD_X - 1,    "╰────────────────────╯");


    mvaddstr(BOARD_Y +  4, BOARD_X - 12,        "╭─────────╮");
    for (int i = 0; i < 11; i++)
        mvaddstr(BOARD_Y + 5 + i, BOARD_X - 12, "│         │");

    mvaddstr(BOARD_Y + 15, BOARD_X - 12,        "╰─────────╯");

         /* Menu windows */
    menu_main = newwin(12, 20, BOARD_Y + 4, BOARD_X); 
    wattron(menu_main, COLOR_PAIR(WB));
    scrollok(menu_main , 0);
    leaveok(menu_main , 1);
    nodelay(menu_main , 0);
    mvwaddstr(menu_main, 0, 0, "arstarst");
}
