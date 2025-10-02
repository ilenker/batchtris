#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"

typedef void (*event_function_t)(WINDOW *window, char *msg, int y, int x);


typedef enum EventName {
    event_null = 0,
    event_xion_blinds = 1,
    event_xion_fade = 2,
    event_wipe = 3,
    event_message = 4,
    event_stats = 5,
    event_store_window = 20,
    event_recall_window = 21,
    event_pause = 32,
    event_resume = 33,
    event_next_sequence = 69,
} eventname_t;


typedef struct Event {
    eventname_t type;
    event_function_t event;
    void *content;
    int y;
    int x;
    int length;
}event_t;

typedef struct Sequence {
    event_t *frame[32];
    bool active;
} sequence_t;


void ui_make_message(WINDOW *window, char *msg, int y, int x);
