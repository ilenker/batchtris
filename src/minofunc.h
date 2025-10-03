#pragma once

#include <ncurses.h>

typedef enum MinoMotion {
    ROTATE_CW = 0,
    ROTATE_CCW = 1,
    ROTATE_180 = 2,
    MOVE_LEFT = 3,
    MOVE_RIGHT = 4,
    HARD_DROP = 5,
    SOFT_DROP = 6,
    HOLD = 7,
    GRAVITY = 8,    
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
    vec_t v[3];      
    shape_t type;    
    int y;           
    int x;           
    int dir;     
    bool falling;
} mino_t;

void mino_render(char ch);
void mino_rotate(rot_t rot);
mino_t *mino_init(shape_t type);
void mino_reset(shape_t type);
void vector_rotate(vec_t *v, rot_t rot);
int mino_resolve_motion(motion_t motion);

void debug_display(char verbosity);
void test_minos();
