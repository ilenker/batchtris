#pragma once

#include <stdbool.h>
#include <unistd.h>
#include <ncurses.h>
#include "gameloops.h"

#define SUCCESS_NOUPDATE 0
#define SUCCESS_UPDATE   2
#define FAIL_NOUPDATE    3
#define BETS_ARE_OFF    -1

typedef union ItemType {
    int num;
    bool state;
    char control;
} item_type_t;

typedef struct MenuItem {
    char name[16];
    item_type_t setting;
} item_t;

game_state_t mode_select();
void menus_init();
void menus_free();
