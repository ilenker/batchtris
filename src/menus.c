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
WINDOW *stdscr_state;
WINDOW *execute_moves_state;

void menus_init() {
    data_main = calloc(1, sizeof(data_t));
    data_options = calloc(1, sizeof(data_t));
    data_controls = calloc(1, sizeof(data_t));

    items_main = calloc(5, sizeof(item_t*));
    for (int i = 0; i < 5; i++) {
        items_main[i] = calloc(1, sizeof(item_t));
    }
    items_options = calloc(11, sizeof(item_t*)); //TODO change alloc size
    for (int i = 0; i < 11; i++) {
        items_options[i] = calloc(1, sizeof(item_t));
    }
    items_controls = calloc(11, sizeof(item_t*));  //TODO change alloc size
    for (int i = 0; i < 11; i++) {
        items_controls[i] = calloc(1, sizeof(item_t));
    }

    data_main->cursor = 0;
    data_main->items = items_main;
    data_main->len = 5;
    data_main->win = main_win;

    data_options->cursor = 0;
    data_options->items = items_options;
    data_options->len = 4;
    data_options->win = options_win;

    data_controls->cursor = 0;
    data_controls->items = items_controls;
    data_controls->len = 9;
    data_controls->win = controls_win;

    item_init_fields(items_main[0], "thinkexecute", EMPTY, 0);
    item_init_fields(items_main[1], "classic"     , EMPTY, 0);
    item_init_fields(items_main[2], "->options"   , EMPTY, 0);
    item_init_fields(items_main[3], "->controls"  , EMPTY, 0);
    item_init_fields(items_main[4], "quit"        , EMPTY, 0);

    item_init_fields(items_options[0], "gravity"         , VAL  ,  1);
    item_init_fields(items_options[1], "replay delay", VAL  , 50);
    item_init_fields(items_options[2], "use unicode"     , STATE,  0);
    item_init_fields(items_options[3], "colorscheme"     , VAL  ,  0);

    item_init_fields(items_controls[0], "Hold"      , CONTROL, 'a');
    item_init_fields(items_controls[1], "Rotate CCW", CONTROL, 'r');
    item_init_fields(items_controls[2], "Rotate CW" , CONTROL, 's');
    item_init_fields(items_controls[3], "Rotate 180", CONTROL, 't');
    item_init_fields(items_controls[4], "Hard Drop" , CONTROL, ' ');
    item_init_fields(items_controls[5], "Move Left" , CONTROL, 'h');
    item_init_fields(items_controls[6], "Soft Drop" , CONTROL, 'z');
    item_init_fields(items_controls[7], "Move Right", CONTROL, ':');

    items_options[0]->has_tooltip = true;
    items_options[1]->has_tooltip = true;
    items_options[2]->has_tooltip = true;
}

void item_init_fields(item_t *item,
                      char *name,
                      item_type_t type, 
                      int data) {
    strncpy(item->name, name, 16);
    item->type = type;
    item->has_tooltip = false;
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
            wprintw(pmenu->win, "%-14s[%c]", item->name, item->data.control);
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

game_state_t mode_select() { // TODO rename
    #define itemlen strlen(pmenu->items[pmenu->cursor]->name)
    #define curitem pmenu->items[pmenu->cursor]
    windows_save();
    nodelay(board_state, 0);
    leaveok(board_state, 1);

    all_delay_set(false);
    keypad(main_win, 1);
    keypad(options_win, 1);
    keypad(controls_win, 1);
    pmenu = data_main;
    box(pmenu->win, 0, 0);
    mvwprintw(pmenu->win, 0, 0, "%s", pmenu->items[0]->name);

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
                }
                if (curitem->type == STATE) {
                    curitem->data.state = !curitem->data.state;
                }
                print_item(curitem, pmenu->cursor + 1, 1);
                break;

            case KEY_RIGHT:
                if (curitem->type == VAL) {
                    curitem->data.val++; 
                }
                if (curitem->type == STATE) {
                    curitem->data.state = !curitem->data.state;
                }
                print_item(curitem, pmenu->cursor + 1, 1);
                break;

            case ' ':
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
                if (strcmp(pmenu->items[pmenu->cursor]->name, "quit") == 0) {
                    cleanup();
                    endwin();
                    exit(0); 
                }
            if (curitem->type == CONTROL) {
                        mvwprintw(input_move_window, 0, 0, "input new key");
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
        if (curitem->has_tooltip) {
            box(ttbox_win, 0, 0);
            wnoutrefresh(ttbox_win);
            print_tooltip(curitem);
        } else {
            werase(tooltip_win);
            wnoutrefresh(tooltip_win);
        }
    }
    return GAME_STATE;
}

char *keyname_get(int keyval) {
    switch (keyval) {
        case 259:
            return "up";
        case KEY_DOWN:
            return "down";
        case KEY_LEFT:
            return "left";
        case KEY_RIGHT:
            return "right";
        case 32:
            return "spcbr";
        case KEY_IC:
            return "ins";
        case KEY_PPAGE:
            return "pgup";
        case KEY_NPAGE:
            return "pgdwn";
        case KEY_HOME:
            return "home";
        case KEY_DC:
            return "del";
        case KEY_BACKSPACE:
            return "bkspc";
        default:
            return "invalid";
    }
}

void print_tooltip(item_t *item) {
    if (strcmp(item->name, "gravity") == 0) {
        werase(tooltip_win);
        mvwprintw(tooltip_win, 0, 0, "Set between 0-20. 0 is off. 20 = 1 cell/frame (~16ms)");
        wnoutrefresh(tooltip_win);
        return;
    }
    if (strcmp(item->name, "use unicode") == 0) {
        werase(tooltip_win);
        mvwprintw(tooltip_win, 0, 0, "If the glyphs in !think! mode overlap, set to off for regular ascii display.");
        wnoutrefresh(tooltip_win);
        return;
    }
    if (strcmp(item->name, "replay delay") == 0) {
        werase(tooltip_win);
        mvwprintw(tooltip_win, 0, 0, "Delay (in ms) between motions during execute phase. Larger ms = slower");
        wnoutrefresh(tooltip_win);
        return;
    }
    if (strcmp(item->name, "replay delay(ms)") == 0) {
        werase(tooltip_win);
        mvwprintw(tooltip_win, 0, 0, "How fast motions replay. Larger ms = slower");
        wnoutrefresh(tooltip_win);
        return;
    }
    werase(tooltip_win);
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
    board_state = dupwin(board_win); // TODO possibly need to dupe input win too
    stdscr_state = dupwin(stdscr); // TODO possibly need to dupe input win too
    execute_moves_state = dupwin(execute_move_window); // TODO possibly need to dupe input win too
}

void windows_restore() {
    wclear(ttbox_win);
    wrefresh(ttbox_win);
    touchwin(board_state);
    touchwin(execute_moves_state);
    wrefresh(board_state);
    wrefresh(execute_moves_state);
}
