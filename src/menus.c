#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "gameloops.h"
#include "init_think_execute.h"
#include "menus.h"

//typedef union ItemType {
//    int num;
//    bool state;
//    char control;
//} item_type_t;
//
//typedef struct MenuItem {
//    char name[16];
//    item_type_t setting;
//} item_t;

item_t **items_main;
item_t **items_options;
item_t **items_controls;


void menus_init() {
    items_main = calloc(11, sizeof(item_t*));
    for (int i = 0; i < 11; i++) {
        items_main[i] = calloc(1, sizeof(item_t));
    }
    items_options = calloc(11, sizeof(item_t*));
    for (int i = 0; i < 11; i++) {
        items_options[i] = calloc(1, sizeof(item_t));
    }
    items_controls = calloc(11, sizeof(item_t*));
    for (int i = 0; i < 11; i++) {
        items_controls[i] = calloc(1, sizeof(item_t));
    }


           //0123456789abcdef   
    strncpy(items_main[0]->name, "thinkexecute" , 16);
    strncpy(items_main[1]->name, "classic"      , 16);
    strncpy(items_main[2]->name, "options"      , 16);
    strncpy(items_main[3]->name, "controls"     , 16);
    strncpy(items_main[4]->name, "quit"         , 16);
    strncpy(items_main[5]->name, ""             , 16);
    strncpy(items_main[6]->name, ""             , 16);
    strncpy(items_main[7]->name, ""             , 16);
    strncpy(items_main[8]->name, ""             , 16);
    strncpy(items_main[9]->name, ""             , 16);

/*
    strncpy("initial gravity", items_options[0]->name, 16);
                               items_options[0]->setting.num = 500;
    strncpy("execution speed", items_options[1]->name, 16);
                               items_options[1]->setting.num = 50;
    strncpy("test pls ignore", items_options[2]->name, 16);
                               items_options[2]->setting.state = false;
    strncpy("raytracing"     , items_options[3]->name, 16);
                               items_options[3]->setting.state = true;
    strncpy("hard mode"      , items_options[4]->name, 16);
                               items_options[4]->setting.state = true;
    strncpy(""               , items_options[5]->name, 16);
    strncpy(""               , items_options[6]->name, 16);
    strncpy(""               , items_options[7]->name, 16);
    strncpy(""               , items_options[8]->name, 16);
    strncpy(""               , items_options[9]->name, 16);

    strncpy("Hold"           , items_controls[0]->name, 16);
                               items_controls[0]->setting.control = 'a'; 
    strncpy("Rotate CW"      , items_controls[1]->name, 16);
                               items_controls[1]->setting.control = 'r'; 
    strncpy("Rotate CCW"     , items_controls[2]->name, 16);
                               items_controls[2]->setting.control = 's'; 
    strncpy("Rotate 180"     , items_controls[3]->name, 16);
                               items_controls[3]->setting.control = 't'; 
    strncpy("Hard Drop"      , items_controls[4]->name, 16);
                               items_controls[4]->setting.control = ' '; 
    strncpy("Move Left"      , items_controls[5]->name, 16);
                               items_controls[5]->setting.control = 'h'; 
    strncpy("Soft Drop"      , items_controls[6]->name, 16);
                               items_controls[6]->setting.control = 'z'; 
    strncpy("Move Right"     , items_controls[7]->name, 16);
                               items_controls[7]->setting.control = ':'; 
    strncpy(""               , items_controls[8]->name, 16);
    strncpy(""               , items_controls[9]->name, 16);*/
}

void menus_free() {
    //free(items_main);  
    //free(items_options);
    //free(items_controls);
}

            /* qwfpmode */
game_state_t mode_select() {
    static unsigned int select = 0;
    WINDOW *screen_state = dupwin(board_win);
    nodelay(screen_state, 0);
    leaveok(screen_state, 1);
    all_delay_set(false);

    box(menu_main, 0, 0);
    mvwaddstr(menu_main, 0, 0, "hello world");

    for (int i = 0; i < 9; i++) {
        wmove(menu_main, i + 1, 1);
        wprintw(menu_main, "%-16s%d\n", items_main[i]->name, items_main[i]->setting);
    }

    wgetch(menu_main);
    all_delay_set(true);
    touchwin(screen_state);
    wrefresh(screen_state);
    delwin(screen_state);
    return CLASSIC;
}
