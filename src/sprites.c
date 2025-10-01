#include <ncurses.h>
#include "sprites.h"
#include "minofunc.h"

void sprite_draw_yx(WINDOW *sprites, sprite_t *s, int y, int x) {
    prefresh(sprites, s->memy, s->memx, y, x, y + s->height - 1, x + s->width - 1);                          
}

void sprites_minos_init(WINDOW *sprites, sprite_t *sprite_data) {
    int sprite_height = 2;

    // I
    // 
    // #### 
    int y_offset = 0;
    wattrset(sprites, COLOR_PAIR(1));
    mvwaddstr(sprites, 1 + y_offset, 0, "#.#.#.#.");
    
    // O
    // ##
    // ## 
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(2));
    mvwaddstr(sprites,     y_offset, 2, "#.#.");
    mvwaddstr(sprites, 1 + y_offset, 2, "#.#.");

    // J
    // #
    // ###
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(3));
    mvwaddstr(sprites,     y_offset, 1, "#.");
    mvwaddstr(sprites, 1 + y_offset, 1, "#.#.#.");

    // L
    //   #
    // ###
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(4));
    mvwaddstr(sprites,     y_offset, 5,     "#.");
    mvwaddstr(sprites, 1 + y_offset, 1, "#.#.#.");

    // S
    //  ##
    // ##
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(5));
    mvwaddstr(sprites,     y_offset, 3,   "#.#.");
    mvwaddstr(sprites, 1 + y_offset, 1, "#.#.");

    // Z
    // ##
    //  ##
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(6));
    mvwaddstr(sprites,     y_offset, 1, "#.#.");
    mvwaddstr(sprites, 1 + y_offset, 3,   "#.#.");

    // T
    //  #
    // ###
    y_offset += sprite_height; 
    wattrset(sprites, COLOR_PAIR(7));
    mvwaddstr(sprites,     y_offset, 3,   "#.");
    mvwaddstr(sprites, 1 + y_offset, 1, "#.#.#.");

    for (int i = 1; i <= 7; i++) {
        sprite_data[i].memy = (i - 1) * 2;
        sprite_data[i].memx = 0;
        sprite_data[i].height = 2;
        sprite_data[i].width = 8;
    }
}
