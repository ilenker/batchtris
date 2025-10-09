#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "gameloops.h"
#include "init_think_execute.h"
#include "menus.h"

void item_init_fields(item_t *item, char *name, item_type_t type, int data); 
void print_item(item_t *item, int y, int x);
void cleanup();
void windows_save();
void windows_restore();

item_t **items_main;
item_t **items_options;
item_t **items_controls;
item_t **current_items;
data_t *pmenu;
data_t *data_main;
WINDOW *board_state;
WINDOW *execute_moves_state;

void menus_init() {
    data_main = calloc(1, sizeof(data_t));
    data_options = calloc(1, sizeof(data_t));
    data_controls = calloc(1, sizeof(data_t));

    items_main = calloc(5, sizeof(item_t*));
    for (int i = 0; i < 6; i++) {
        items_main[i] = calloc(1, sizeof(item_t));
    }
    items_options = calloc(4, sizeof(item_t*)); //TODO change alloc size
    for (int i = 0; i < 4; i++) {
        items_options[i] = calloc(1, sizeof(item_t));
    }
    items_controls = calloc(8, sizeof(item_t*));  //TODO change alloc size
    for (int i = 0; i < 8; i++) {
        items_controls[i] = calloc(1, sizeof(item_t));
    }

    data_main->cursor = 0;
    data_main->items = items_main;
    data_main->win = main_win;

    data_options->cursor = 0;
    data_options->items = items_options;
    data_options->win = options_win;

    data_controls->cursor = 0;
    data_controls->items = items_controls;
    data_controls->win = controls_win;

    item_init_fields(items_main[0], "thinkexecute", EMPTY, 0);
    item_init_fields(items_main[1], "classic"     , EMPTY, 0);
    item_init_fields(items_main[2], "->options"   , EMPTY, 0);
    item_init_fields(items_main[3], "->controls"  , EMPTY, 0);
    item_init_fields(items_main[4], "reset"       , EMPTY, 0);
    item_init_fields(items_main[5], "quit"        , EMPTY, 0);
    data_main->len = 6;

    item_init_fields(items_options[0], "gravity"         , VAL  ,  1);
    data_options->items[0]->u_limit = 20;
    data_options->items[0]->l_limit = 0;
    item_init_fields(items_options[1], "replay delay"    , VAL  , 50);
    data_options->items[1]->u_limit = 500;
    data_options->items[1]->l_limit = 0;
    item_init_fields(items_options[2], "use unicode"     , STATE,  1);
    item_init_fields(items_options[3], "stndrd colours" , STATE,  0);
    data_options->len = 4;

    item_init_fields(items_controls[0], "Hold"      , CONTROL, 'z');
    item_init_fields(items_controls[1], "Rotate CCW", CONTROL, 'x');
    item_init_fields(items_controls[2], "Rotate CW" , CONTROL, 'c');
    item_init_fields(items_controls[3], "Rotate 180", CONTROL, 'v');
    item_init_fields(items_controls[4], "Hard Drop" , CONTROL, ' ');
    item_init_fields(items_controls[5], "Move Left" , CONTROL, 4);
    item_init_fields(items_controls[6], "Soft Drop" , CONTROL, 2);
    item_init_fields(items_controls[7], "Move Right" , CONTROL,5);
    items_controls[0]->data_default.control = 'a';
    items_controls[1]->data_default.control = 'r';
    items_controls[2]->data_default.control = 's';
    items_controls[3]->data_default.control = 't';
    items_controls[4]->data_default.control = ' ';
    items_controls[5]->data_default.control = 'h';
    items_controls[6]->data_default.control = 'z';
    items_controls[7]->data_default.control = ':';
    //item_init_fields(items_controls[8], "Preset Map:", VAL, 0);
    //data_controls->items[8]->u_limit = 2;
    //data_controls->items[8]->l_limit = 0;
    data_controls->len = 8;

}

void item_init_fields(item_t *item,
                      char *name,
                      item_type_t type, 
                      int data) {
    strncpy(item->name, name, 16);
    item->type = type;
    item->has_tooltip = true;
    switch (type) {
        case VAL:
            item->data.val = data;
            item->data_default.val = (int)data;
            break;
        case STATE:
            item->data.state = data;
            item->data_default.state = (bool)data;
            break;
        case CONTROL:
            item->data.control = data;
            item->data_default.control = (char)data;
            break;
        case EMPTY:
            item->data.val = -1;
            item->data_default.val = (int)data;
            break;
    }
}

void print_item(item_t *item, int y, int x) {
    char *state;

    switch (item->type) {
        case VAL:
            wmove(pmenu->win, y, x);
            wprintw(pmenu->win, "%-14s%3d", item->name, item->data.val);
            return;
        case STATE:
            wmove(pmenu->win, y, x);
            state = (item->data.state == true) ? "On" : "Off";
            wprintw(pmenu->win, "%-14s%3s", item->name, state);
            return;
        case CONTROL:
            wmove(pmenu->win, y, x);
            if (item->data.control > 32 && item->data.control < 127) {
                wprintw(pmenu->win, "%-13s%c", item->name, item->data.control);
            }
            else if (item->data.control == 32) {
                wprintw(pmenu->win, "%-13sSPC", item->name);
            } else {
                switch (item->data.control) {
                    case 3:
                        wprintw(pmenu->win, "%-13sUP", item->name);
                        break;
                    case 2:
                        wprintw(pmenu->win, "%-13sDOWN", item->name);
                        break;
                    case 4:
                        wprintw(pmenu->win, "%-13sLEFT", item->name);
                        break;
                    case 5:
                        wprintw(pmenu->win, "%-13sRIGHT", item->name);
                        break;
                }
            }
            return;
        case EMPTY:
            wmove(pmenu->win, y, x);
            if (strcmp(item->name, "") == 0) {
                wprintw(pmenu->win, "\n");
                wclrtoeol(pmenu->win);
                return;
            }
            wprintw(pmenu->win, "%s", item->name);
            return;
    }
}

game_state_t mode_select() { 
    #define itemlen strlen(pmenu->items[pmenu->cursor]->name)
    #define curitem pmenu->items[pmenu->cursor]
    windows_save();

    box(ttbox_win, 0, 0);
    wnoutrefresh(ttbox_win);

    all_delay_set(false);
    keypad(main_win, 1);
    keypad(options_win, 1);
    keypad(controls_win, 1);

    pmenu = data_main;
    box(pmenu->win, 0, 0);

        // Print menu items
    for (int i = 0; i < pmenu->len; i++) {
        wmove(pmenu->win, i + 1, 1);
        print_item(pmenu->items[i], i + 1, 1);
    }

    wchgat(pmenu->win, itemlen, !A_STANDOUT, 8, NULL);
    wmove(pmenu->win, 1 + pmenu->cursor, 1);
    wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);

    while (true) {
        int c = wgetch(pmenu->win);
        wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
        switch (c) {
            case 27:
                if (pmenu != data_main) {
                    pmenu  = data_main;
                    for (int i = 0; i < pmenu->len; i++) {
                        wmove(pmenu->win, i + 1, 1);
                        print_item(pmenu->items[i], i + 1, 1);
                    }
                    wmove(pmenu->win, 1 + pmenu->cursor, 1);
                    wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
                    box(pmenu->win, 0, 0);
                    touchwin(pmenu->win);
                } else {
                    if (PREV_GAME_STATE != MENU) {
                        all_delay_set(true);
                        windows_restore();
                        return PREV_GAME_STATE;
                    }
                }
                break;

            case KEY_DOWN:
                wchgat(pmenu->win, itemlen, !A_STANDOUT, 8, NULL);
                pmenu->cursor = (pmenu->cursor + 1) % pmenu->len;
                wmove(pmenu->win, 1 + pmenu->cursor, 1);
                wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
                break;

            case KEY_UP:
                wchgat(pmenu->win, itemlen, !A_STANDOUT, 8, NULL);
                pmenu->cursor = (pmenu->cursor - 1 + pmenu->len) % pmenu->len;
                wmove(pmenu->win, 1 + pmenu->cursor, 1);
                wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
                break;

            case KEY_LEFT:
                if (curitem->type == VAL) {
                    curitem->data.val--; 
                    if (curitem->data.val < curitem->l_limit) {
                        curitem->data.val = curitem->u_limit;
                    }
                    print_item(curitem, pmenu->cursor + 1, 1);
                }
                if (curitem->type == STATE) {
                    curitem->data.state = !curitem->data.state;
                    print_item(curitem, pmenu->cursor + 1, 1);
                    if (pmenu == data_options) {
                        if (pmenu->cursor == 3) {
                            colors_init(data_options->items[3]->data.state);
                            doupdate();
                        }
                    }
                }
                break;

            case KEY_RIGHT:
                if (curitem->type == VAL) {
                    curitem->data.val++; 
                    if (curitem->data.val > curitem->u_limit) {
                        curitem->data.val = curitem->l_limit;
                    }
                    print_item(curitem, pmenu->cursor + 1, 1);
                }
                if (curitem->type == STATE) {
                    curitem->data.state = !curitem->data.state;
                    print_item(curitem, pmenu->cursor + 1, 1);
                    if (pmenu == data_options) {
                        if (pmenu->cursor == 3) {
                            colors_init(data_options->items[3]->data.state);
                            doupdate();
                        }
                    }
                }
                break;

            case ' ':
                if (strcmp(pmenu->items[pmenu->cursor]->name, "thinkexecute") == 0) {
                    all_delay_set(true);
                    windows_restore();
                    return THINK;
                }
                if (strcmp(pmenu->items[pmenu->cursor]->name, "classic") == 0) {
                    all_delay_set(true);
                    windows_restore();
                    return CLASSIC;
                }
                if (strcmp(pmenu->items[pmenu->cursor]->name, "->options") == 0) {
                    werase(pmenu->win);
                    pmenu = data_options;
                    for (int i = 0; i < pmenu->len; i++) {
                        wmove(pmenu->win, i + 1, 1);
                        print_item(pmenu->items[i], i + 1, 1);
                    }
                    wmove(pmenu->win, 1 + pmenu->cursor, 1);
                    wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
                    box(pmenu->win, 0, 0);
                    touchwin(pmenu->win);
                }
                if (strcmp(pmenu->items[pmenu->cursor]->name, "->controls") == 0) {
                    werase(pmenu->win);
                    pmenu = data_controls;
                    box(pmenu->win, 0, 0);
                    for (int i = 0; i < pmenu->len; i++) {
                        wmove(pmenu->win, i + 1, 1);
                        print_item(pmenu->items[i], i + 1, 1);
                    }
                    wmove(pmenu->win, 1 + pmenu->cursor, 1);
                    wchgat(pmenu->win, itemlen, A_STANDOUT, 8, NULL);
                    box(pmenu->win, 0, 0);
                    touchwin(pmenu->win);
                    break;
                }
                if (strcmp(pmenu->items[pmenu->cursor]->name, "reset") == 0) {
                    cleanup();
                    endwin();
                    return RESET;
                }
                if (strcmp(pmenu->items[pmenu->cursor]->name, "quit") == 0) {
                    cleanup();
                    endwin();
                    exit(0); 
                }
            if (curitem->type == CONTROL) {
                        wrefresh(input_move_window);
                        char new_bind = wgetch(pmenu->win);
                            if (new_bind == 27) {
                            mvwprintw(input_move_window, 0, 0, "invalid key");
                            wrefresh(input_move_window);
                        } else {
                            curitem->data.control = new_bind;
                            print_item(curitem, pmenu->cursor + 1, 1);
                        } 
                }
                break;
        }
        print_tooltip();
    }
    return GAME_STATE;
}

void print_tooltip() {
    werase(tooltip_win);
    if (pmenu == data_main) {
        switch (pmenu->cursor) {
            case 0:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "      !think! !execute!       ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Input the moves for each piece");
                mvwprintw(tooltip_win, 3, 0,       "in the queue displayed on the ");
                mvwprintw(tooltip_win, 4, 0,       "right of the board.           ");
                mvwprintw(tooltip_win, 5, 0,       "                              ");
                mvwprintw(tooltip_win, 6, 0,       "Your inputs are shown on the  ");
                mvwprintw(tooltip_win, 7, 0,       "left of the board, as a string");
                mvwprintw(tooltip_win, 8, 0,       "of symbols (arrows, rotations ");
                mvwprintw(tooltip_win, 9, 0,       "hold, drops).                 ");
                mvwprintw(tooltip_win,10, 0,       "                              ");
                mvwprintw(tooltip_win,11, 0,       "After you do the last piece,  ");
                mvwprintw(tooltip_win,12, 0,       "the sequence executes, and we ");
                mvwprintw(tooltip_win,13, 0,       "start the loop again.         ");
                break;

            case 1:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "     classic block stacker    ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Basic spins only (t, l, j)    ");
                mvwprintw(tooltip_win, 3, 0,       "No kick tables                ");
                mvwprintw(tooltip_win, 4, 0,       "                              ");
                mvwprintw(tooltip_win, 5, 0,       "Press <1> to open map editor. ");
                mvwprintw(tooltip_win, 6, 0,       "Right mouse buttn picks colour");
                mvwprintw(tooltip_win, 7, 0,       "Left to place colour on board ");
                mvwprintw(tooltip_win, 8, 0,       "Pick an empty cell to erase   ");
                mvwprintw(tooltip_win, 9, 0,       "cells.                        ");
                break;

            case 2:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "          Options             ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Set gravity, replay speed,    ");
                mvwprintw(tooltip_win, 3, 0,       "display options.              ");
                break;

            case 3:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "          Controls            ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Reassign keys.                ");
                break;

            case 4:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "            Reset             ");
                break;

            case 5:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "            Quit              ");
                break;
        }

    }

    if (pmenu == data_options) {
        switch (pmenu->cursor) {
            case 0:                             //  ############################## 
                mvwprintw(tooltip_win, 0, 0,       "Set between 0 and 20.         ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "0 = Gravity off               ");
                mvwprintw(tooltip_win, 3, 0,       "20 = 1 cell/frame (~16ms)     ");
                break;
            case 1:
                mvwprintw(tooltip_win, 0, 0,       "How long to wait after each   ");
                mvwprintw(tooltip_win, 1, 0,       "motion in the replay phase    ");
                mvwprintw(tooltip_win, 2, 0,       "of !think execute!.           ");
                mvwprintw(tooltip_win, 3, 0,       "                              ");
                mvwprintw(tooltip_win, 4, 0,       "Delay in milliseconds, larger ");
                mvwprintw(tooltip_win, 5, 0,       "values = slower replay.       ");
                break;
            case 2:
                mvwprintw(tooltip_win, 0, 0,       "Set to off if you have any    ");
                mvwprintw(tooltip_win, 1, 0,       "issues with how the input     ");
                mvwprintw(tooltip_win, 2, 0,       "symbols are displayed.        ");
                mvwprintw(tooltip_win, 3, 0,       "                              ");
                mvwprintw(tooltip_win, 4, 0,       "              [On]    [Off]   ");
                mvwprintw(tooltip_win, 5, 0,       " left:         ←       <      ");
                mvwprintw(tooltip_win, 6, 0,       " right:        →       >      ");
                mvwprintw(tooltip_win, 7, 0,       " harddrop:     !       !      ");
                mvwprintw(tooltip_win, 8, 0,       " rotate cw:    ↷       C      ");
                mvwprintw(tooltip_win, 9, 0,       " rotate ccw:   ↶       @      ");
                mvwprintw(tooltip_win,10, 0,       " rotate 180:   π       8      ");
                mvwprintw(tooltip_win,11, 0,       " soft drop:    ⌄       v      ");
                mvwprintw(tooltip_win,12, 0,       " hold:         HOLD    HOLD   ");
                break;
            case 3:
                mvwprintw(tooltip_win, 0, 0,       "If your terminal only displays");
                mvwprintw(tooltip_win, 1, 0,       "8 colours, toggle this on.    ");
                break;
        }
    }

    if (pmenu == data_controls) {
        switch (pmenu->cursor) {
            case 0:
                mvwprintw(tooltip_win, 0, 0,       "            Hold:             ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Swap active piece with the    ");
                mvwprintw(tooltip_win, 3, 0,       "piece currently held (shown   ");
                mvwprintw(tooltip_win, 4, 0,       "top left of the board)        ");
                mvwprintw(tooltip_win, 5, 0,       "                              ");
                break;

            case 1:
                mvwprintw(tooltip_win, 0, 0,       "        Rotate Counter-       ");
                mvwprintw(tooltip_win, 1, 0,       "           Clockwise:         ");
                mvwprintw(tooltip_win, 2, 0,       "                              ");
                mvwprintw(tooltip_win, 3, 0,       "Rotate piece 90° ccw.         ");
                mvwprintw(tooltip_win, 4, 0,       "Choosing the optimal rotation ");
                mvwprintw(tooltip_win, 5, 0,       "is crucial for playing with   ");
                mvwprintw(tooltip_win, 6, 0,       "perfect finesse.              ");
                break;

            case 2:
                mvwprintw(tooltip_win, 0, 0,       "      Rotate Clockwise:       ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Rotate piece 90° cw.          ");
                mvwprintw(tooltip_win, 3, 0,       "You only ever need, at most,  ");
                mvwprintw(tooltip_win, 4, 0,       "*1 rotation* for any placement");
                mvwprintw(tooltip_win, 5, 0,       "(except for various spins)    ");
                break;

            case 3:
                mvwprintw(tooltip_win, 0, 0,       "         Rotate 180°:         ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Rotate piece 180°.            ");
                mvwprintw(tooltip_win, 3, 0,       "Just another rotation option, ");
                mvwprintw(tooltip_win, 4, 0,       "Use instead of 2x of the 90°  ");
                mvwprintw(tooltip_win, 5, 0,       "options.                      ");
                mvwprintw(tooltip_win, 6, 0,       "Also used for L/J spin.       ");
                break;

            case 4:
                mvwprintw(tooltip_win, 0, 0,       "          Hard Drop:          ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Drop and place current piece  ");
                mvwprintw(tooltip_win, 3, 0,       "instantly.                    ");
                break;

            case 5:
                mvwprintw(tooltip_win, 0, 0,       "         Move Left:           ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Move the piece left, of course");
                mvwprintw(tooltip_win, 3, 0,       "                              ");
                mvwprintw(tooltip_win, 4, 0,       "As with rotations, there's an ");
                mvwprintw(tooltip_win, 5, 0,       "absolute max limit of movement");
                mvwprintw(tooltip_win, 6, 0,       "inputs needed for any piece   ");
                mvwprintw(tooltip_win, 7, 0,       "placement (only exceptions are");
                mvwprintw(tooltip_win, 8, 0,       "slides)                       ");
                break;

            case 6:
                mvwprintw(tooltip_win, 0, 0,       "          Soft Drop:          ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Same as Hard, but without     ");
                mvwprintw(tooltip_win, 3, 0,       "placing instantly, allowing   ");
                mvwprintw(tooltip_win, 4, 0,       "for slides.                   ");
                mvwprintw(tooltip_win, 5, 0,       "                              ");
                mvwprintw(tooltip_win, 6, 0,       "Because each new key overrides");
                mvwprintw(tooltip_win, 7, 0,       "the last, if you hold down e.g");
                mvwprintw(tooltip_win, 8, 0,       "left, if you rotate while the ");
                mvwprintw(tooltip_win, 9, 0,       "piece is moving, the leftward ");
                mvwprintw(tooltip_win,10, 0,       "movement will just stop.      ");
                mvwprintw(tooltip_win,11, 0,       "Feels pretty janky, but it's  ");
                mvwprintw(tooltip_win,12, 0,       "still possible to play plenty ");
                mvwprintw(tooltip_win,13, 0,       "fast once you get used to it. ");
                break;

            case 7:
                mvwprintw(tooltip_win, 0, 0,       "         Move Right:          ");
                mvwprintw(tooltip_win, 1, 0,       "                              ");
                mvwprintw(tooltip_win, 2, 0,       "Move the piece rightward.     ");
                mvwprintw(tooltip_win, 3, 0,       "                              ");
                mvwprintw(tooltip_win, 4, 0,       "Unfortunately, this game does ");
                mvwprintw(tooltip_win, 5, 0,       "not allow for simultaneous key");
                mvwprintw(tooltip_win, 6, 0,       "presses, and does not register");
                mvwprintw(tooltip_win, 7, 0,       "key release information   -   ");
                mvwprintw(tooltip_win, 8, 0,       "This means that the DAS / ARR ");
                mvwprintw(tooltip_win, 9, 0,       "of this game is determined by ");
                mvwprintw(tooltip_win,10, 0,       "your system settings (how fast");
                mvwprintw(tooltip_win,11, 0,       "your keys repeat when heldddd)");
                break;

            //case 8:
            //    mvwprintw(tooltip_win, 0, 0,       "       Preset Keymaps:        ");
            //    mvwprintw(tooltip_win, 1, 0,       "                              ");
            //    mvwprintw(tooltip_win, 2, 0,       "0: Default (zxcv_arrows)      ");
            //    mvwprintw(tooltip_win, 3, 0,       "   most tetris games use this ");
            //    mvwprintw(tooltip_win, 4, 0,       "   or similar.                ");
            //    mvwprintw(tooltip_win, 5, 0,       "                              ");
            //    mvwprintw(tooltip_win, 6, 0,       "1: colemak homerow (arst_nei) ");
            //    mvwprintw(tooltip_win, 7, 0,       "                              ");
            //    mvwprintw(tooltip_win, 8, 0,       "2: qwerty  homerow (asdf_jkl) ");
            //    break;
        }
    }
    wnoutrefresh(tooltip_win);
    return;
}

void cleanup() {
    free(items_main);  
    free(items_options);
    free(items_controls);
    free(data_main);
    free(data_controls);
    free(data_options);
    free(mino);
    free(board);
    board_init_sll(true);
    delwin(board_win);
    delwin(sprites);
    delwin(input_move_window);
    delwin(execute_move_window);
    delwin(debug_window);
    delwin(main_win);
    delwin(options_win);
    delwin(controls_win);
    delwin(tooltip_win);
}

void windows_save() {
    board_state = dupwin(board_win); 
    execute_moves_state = dupwin(execute_move_window);
}

void windows_restore() {
    wclear(ttbox_win);
    wrefresh(ttbox_win);
    touchwin(board_state);
    touchwin(execute_moves_state);
    wrefresh(board_state);
    wrefresh(execute_moves_state);
    delwin(board_state);
    delwin(execute_moves_state);
}
