#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef struct Vector {
    char dy; 
    char dx;
} vec_t;

typedef struct Mino {
    char y;
    char x;
    char rot;
    char type;
    vec_t v[3];
} mino_t;

char get_mino_color(char type); 
void render_mino(mino_t *mino, char ch);
void rotate_mino(mino_t *mino, char dir);
mino_t *make_mino(char type);
void test_minos();
void rotate_vector(vec_t *v, char dir);
