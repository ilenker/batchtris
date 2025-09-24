#pragma once

#include <ncurses.h>
#include <stdlib.h>

typedef enum GlobalGameState {
    MENU,
    INPUT_MOVES,
    EXECUTE_MOVES,
    RESULTS,
} globalstate_t;

typedef enum Rotation {
    rCW = -1,
    rCCW = 1,
    r180 = 2,
} rot_t;

typedef enum ShapeType {
    I = 1,
    O = 2,
    J = 3,
    L = 4,
    S = 5,
    Z = 6,
    T = 7,
} shape_t;

typedef struct Vector {
    char dy; 
    char dx;
} vec_t;

typedef struct Mino {
    vec_t v[3];
    char y;
    char x;
    unsigned char rot;
    shape_t type;
} mino_t;

void render_mino(mino_t *mino, char ch);
void rotate_mino(mino_t *mino, rot_t rot);
mino_t *make_mino(shape_t type);
void test_minos();
void rotate_vector(vec_t *v, rot_t rot);
void debug_display(mino_t *mino, char verbosity);
