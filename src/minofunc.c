#include <ncurses.h>
#include <stdlib.h>
#include "minofunc.h"

vec_t VERY_SPECIAL_TABLE[4] = {
    {0 , 1},
    {1 , 0},
    {0 ,-1},
    {-1, 0}
};

mino_t *make_mino(shape_t type) {
    mino_t *new_mino = malloc(sizeof(mino_t));
    if (new_mino == NULL) {
        return NULL;
    }
    new_mino->type = type;
    new_mino->rot = 0;
    new_mino->y = 8;
    new_mino->x = 8;
    switch (type) {
        case I:
            new_mino->v[0].dy = 0; 
            new_mino->v[0].dx = -1; 
            new_mino->v[1].dy = 0;          
            new_mino->v[1].dx = 1;       // oxoo
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 2; 
            break;
        case O:
            new_mino->v[0].dy = -1;      //  oo 
            new_mino->v[0].dx = 0;      //   xo
            new_mino->v[1].dy = -1; 
            new_mino->v[1].dx = 1; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case J:
            new_mino->v[0].dy = -1;     // o
            new_mino->v[0].dx = -1;    //  oxo
            new_mino->v[1].dy = 0; 
            new_mino->v[1].dx = -1; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case L:
            new_mino->v[0].dy = 0;      //   o
            new_mino->v[0].dx = -1;    //  oxo
            new_mino->v[1].dy = 0; 
            new_mino->v[1].dx = 1; 
            new_mino->v[2].dy = -1; 
            new_mino->v[2].dx = 1; 
            break;
        case S:
            new_mino->v[0].dy = 0; 
            new_mino->v[0].dx = -1;     //  oo
            new_mino->v[1].dy = -1;    //  ox
            new_mino->v[1].dx = 0; 
            new_mino->v[2].dy = -1; 
            new_mino->v[2].dx = 1; 
            break;
        case Z:
            new_mino->v[0].dy = -1; 
            new_mino->v[0].dx = -1;     // oo
            new_mino->v[1].dy = -1;    //   xo
            new_mino->v[1].dx = 0; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case T:
            new_mino->v[0].dy = 0; 
            new_mino->v[0].dx = -1;     // o
            new_mino->v[1].dy = -1;    // oxo
            new_mino->v[1].dx = 0; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
    }
    return new_mino;
}

void debug_display(mino_t *mino, char verbosity) {
    attron(COLOR_PAIR(9));
    switch (verbosity) {
        case 1:
            mvprintw(4, 3, "[0]y: %d ", mino->v[0].dy);
            mvprintw(4, 11, "| [0]x: %d ", mino->v[0].dx);
            mvprintw(5, 3, "[1]y: %d ", mino->v[1].dy);
            mvprintw(5, 11, "| [1]x: %d ", mino->v[1].dx);
            mvprintw(6, 3, "[2]y: %d ", mino->v[2].dy);
            mvprintw(6, 11, "| [2]x: %d ", mino->v[2].dx);
        case 0:
            mvprintw(1, 3, "type: %d ", mino->type);
            mvprintw(2, 3, "rot: %d ", mino->rot);
            mvprintw(3, 3, "y: %d ", mino->y);
            mvprintw(3, 11, "| x: %d ", mino->x);
    }
}

void render_mino(mino_t *mino, char ch) {
    char col = ch == '0' ? 8 : mino->type;
    attron(COLOR_PAIR(col));

    mvaddch(mino->y, mino->x * 2, ch);
    addch(ch);
    for (char i = 0; i < 3; i++) {      
        mvaddch(mino->y + mino->v[i].dy,
                 (mino->x + mino->v[i].dx) * 2,
                 ch);
        addch(ch);
    }
}

void rotate_mino(mino_t *mino, rot_t rot) {
              // TODO: rename mino->rot property to something like "dir" or w/e
    if (mino->type == O) {
        return;
    }
          //   the I piece is a special boy so
         //   we need some extra logic to make
        //   it rotate correctly - we move the
       //   position with each rotation event. 
    switch (rot) {
        case rCW:
            if (mino->type == I) {
                mino->y += VERY_SPECIAL_TABLE[mino->rot].dy;
                mino->x += VERY_SPECIAL_TABLE[mino->rot].dx;
            }
            mino->rot = (mino->rot + 1) % 4;
            break;
        case rCCW:
            mino->rot = mino->rot == 0 ? 3 : mino->rot - 1;  
            if (mino->type == I) {
                mino->y -= VERY_SPECIAL_TABLE[mino->rot].dy;
                mino->x -= VERY_SPECIAL_TABLE[mino->rot].dx;
            }
            break;
        case r180:
            if (mino->type == I) {
                mino->y += VERY_SPECIAL_TABLE[mino->rot].dy;
                mino->x += VERY_SPECIAL_TABLE[mino->rot].dx;
                mino->y += VERY_SPECIAL_TABLE[(mino->rot + 1) % 4].dy;
                mino->x += VERY_SPECIAL_TABLE[(mino->rot + 1) % 4].dx;
                mino->rot = (mino->rot + 2) % 4;
                break;
            }
            mino->rot = (mino->rot + 2) % 4;
            break;
    }
    rotate_vector(&mino->v[0], rot);
    rotate_vector(&mino->v[0], rot);
    rotate_vector(&mino->v[0], rot);

    rotate_vector(&mino->v[1], rot);
    rotate_vector(&mino->v[1], rot);
    rotate_vector(&mino->v[1], rot);

    rotate_vector(&mino->v[2], rot);
    rotate_vector(&mino->v[2], rot);
    rotate_vector(&mino->v[2], rot);
}

void rotate_vector(vec_t *v, rot_t rot) {
    char py = v->dy;
    char px = v->dx;
    switch (rot) {
        case rCW:        /* "-(pi/2)", aka 90° clockwise */
            v->dy = -px;
            v->dx = py;
            break;
        case rCCW:       /* "(pi/2)", aka 90° anti-clockwise */
            v->dy = px;
            v->dx = -py;
            break;
        case r180:       /* "2(pi/2)", aka 180° */
            v->dy = -py;
            v->dx = -px;
            break;
    }
}
void rotate_vector_BKP(vec_t *v, char dir) {
    switch (dir) {
        case 2:
            v->dy = -1 * (v->dx * 1) + (v->dy * 0);
            v->dx = (v->dx * 0) + (v->dy * 1);
            break;
        case 1:
            v->dy = -1 * (v->dx * -1) + (v->dy * 0);
            v->dx = (v->dx * 0) + (v->dy * -1);
            break;
        case -1:
            v->dy = -1 * (v->dx * 0) + (v->dy * -1);
            v->dx = (v->dx * -1) + (v->dy * 0);
            break;
    }
}

void test_minos() {
    mino_t *imino = make_mino(I);
    imino->x = 1;
    imino->y = 5;
    render_mino(imino, '1');

    mino_t *omino = make_mino(O);
    omino->x = 10;
    omino->y = 5;
    render_mino(omino, 1);

    mino_t *jmino = make_mino(J);
    jmino->x = 15;
    jmino->y = 5;
    render_mino(jmino, 1);

    mino_t *lmino = make_mino(L);
    lmino->x = 5;
    lmino->y = 10;
    render_mino(lmino, 1);

    mino_t *smino = make_mino(S);
    smino->x = 10;
    smino->y = 10;
    render_mino(smino, 1);

    mino_t *zmino = make_mino(Z);
    zmino->x = 15;
    zmino->y = 10;
    render_mino(zmino, 1);

    mino_t *tmino = make_mino(T);
    tmino->x = 15;
    tmino->y = 15;
    render_mino(tmino, 1);
    free(imino);
    free(omino);
    free(jmino);
    free(lmino);
    free(smino);
    free(zmino);
    free(tmino);
}
