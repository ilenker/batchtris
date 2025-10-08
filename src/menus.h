#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>
#include "gameloops.h"

typedef union ItemData {
    int val;
    char control;
    bool state;
} item_data_union;

typedef enum ItemType {
    VAL,
    STATE,
    CONTROL,
    EMPTY,
} item_type_t;

typedef struct MenuItem {
    char name[16];
    item_type_t type; 
    item_data_union data;
    item_data_union data_default;
    bool has_tooltip;
} item_t;


typedef struct MenuData {
    item_t **items;
    WINDOW *win;
    int cursor;
    int len;
} data_t;


game_state_t mode_select();
void menus_init();
void menus_free();
char *keyname_get(int keyval);
void print_tooltip(item_t *item);

extern data_t *data_options;
extern data_t *data_controls;
