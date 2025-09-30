#pragma once

#include <ncurses.h>

typedef struct Sprite {
    int memy;
    int memx;
    int height;
    int width;
} sprite_t;


void sprites_minos_init(WINDOW *sprites, sprite_t *sprite_data);
void sprite_draw_yx(WINDOW *sprites, sprite_t *sprite, int y, int x);
