#pragma once

#include "boardfunc.h"
#include <ncurses.h>
#include <stdlib.h>

typedef enum GlobalGameState {
    MENU,
    INPUT_MOVES,
    EXECUTE_MOVES,
    RESULTS,
} g_state_t;

typedef enum MinoMotion {
    GRAVITY,
    MOVE_LEFT,
    MOVE_RIGHT,
    ROTATE_CW,
    ROTATE_CCW,
    ROTATE_180,
    HARD_DROP,
    SOFT_DROP
} motion_t;

typedef enum Rotation {
    r270 = -1,
    r90 = 1,
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
    int dy; 
    int dx;
} vec_t;

typedef struct Mino {
    vec_t v[3];             // 6
    shape_t type;           // 4
    int y;                 // 1
    int x;                 // 1
    int dir;      // 1
    bool falling;
} mino_t;

void render_mino(WINDOW *window, mino_t *mino, char ch);
void rotate_mino(mino_t *mino, rot_t rot);
mino_t *make_mino(shape_t type);
void set_mino(mino_t *new_mino, shape_t type);
void rotate_vector(vec_t *v, rot_t rot);
int resolve_mino_motion(board_t *board, mino_t *mino, motion_t motion);

void debug_display(mino_t *mino, board_t *board, char verbosity);
void test_minos();
