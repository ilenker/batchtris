#include <ncurses.h>
#include <stdlib.h>
#include "minofunc.h"
#include "boardfunc.h"

const vec_t g_irotation_lut[4] = {
    {0 , 1},
    {1 , 0},
    {0 ,-1},
    {-1, 0}
};

mino_t *make_mino(shape_t type) {
    mino_t *new_mino = calloc(1, sizeof(mino_t));
    if (new_mino == NULL) {
        return NULL;
    }
    new_mino->type = type;
    new_mino->dir = 0;
    new_mino->y = 3;
    new_mino->x = 4;
    new_mino->falling = true;
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

void resolve_mino_motion(board_t *board, mino_t *mino, motion_t motion) {
    switch (motion) {
        case GRAVITY:       
            if (mino->y > board->render_limit) {
                mino->y++;
                mino->falling = true;
                return;
            }
            if (board->grid[mino->y + 1][mino->x] != 9) {
                mino->falling = false;
                return;
            }
            for (char i = 0; i < 3; i++) {
                if (board->grid[mino->y + mino->v[i].dy + 1]
                               [mino->x + mino->v[i].dx    ] != 9) {
                    mino->falling = false;
                    return;
                }
            }
            mino->y++;
            mino->falling = true;
            return;
        case MOVE_LEFT:
            // TODO
            break;
        case MOVE_RIGHT:
            // TODO
            break;
        case ROTATE_CW:
            // TODO
            break;
        case ROTATE_CCW:
            // TODO
            break;
        case ROTATE_180:
            // TODO
            break;
        case HARD_DROP:
            // TODO
            break;
        case SOFT_DROP:   
            // TODO
            break;
    }
}

void debug_display(mino_t *mino, char verbosity) {
    if (verbosity == 0) {
        return;
    }
    attron(COLOR_PAIR(9));
    switch (verbosity) {
        case 2:
            mvprintw(5, 3, "[0]y: %d ", mino->v[0].dy);
            mvprintw(5, 11, "| [0]x: %d ", mino->v[0].dx);
            mvprintw(6, 3, "[1]y: %d ", mino->v[1].dy);
            mvprintw(6, 11, "| [1]x: %d ", mino->v[1].dx);
            mvprintw(7, 3, "[2]y: %d ", mino->v[2].dy);
            mvprintw(7, 11, "| [2]x: %d ", mino->v[2].dx);
        case 1:
            mvprintw(1, 3, "type: %d ", mino->type);
            mvprintw(2, 3, "dir: %d ", mino->dir);
            mvprintw(3, 3, "y: %d ", mino->y);
            mvprintw(3, 11, "| x: %d ", mino->x);
            mvprintw(4, 3, "falling: %d ", mino->falling);
    }
}

void render_mino(WINDOW * window, mino_t *mino, char ch) {
    char col = ch == '0' ? 8 : mino->type;
    wattron(window, COLOR_PAIR(col));               /* Set the color based on mino type */
    mvwaddch(window, mino->y, mino->x * 2, ch);    /*    x*2 to stretch x res by 2     */
    waddch(window, ch);                           /* print extra ch to fill the space */

    for (char i = 0; i < 3; i++) {      
        mvwaddch(window,                          /* and do the same for each of */
                 mino->y + mino->v[i].dy,        /* minos individual components */
                (mino->x + mino->v[i].dx) * 2,
                 ch);
        waddch(window, ch);
    }
}

void rotate_mino(mino_t *mino, rot_t rot) {
    if (mino->type == O) {
        return;
    }
          //   the I piece is a special boy so
         //   we need some extra logic to make
        //   it rotate correctly - we move the
       //   position with each rotation event. 
    switch (rot) {
        case r270:
            if (mino->type == I) {
                mino->y += g_irotation_lut[mino->dir].dy;
                mino->x += g_irotation_lut[mino->dir].dx;
            }
            mino->dir = (mino->dir + 1) % 4;
            break;
        case r90:
            mino->dir = mino->dir == 0 ? 3 : mino->dir - 1;  
            if (mino->type == I) {
                mino->y -= g_irotation_lut[mino->dir].dy;
                mino->x -= g_irotation_lut[mino->dir].dx;
            }
            break;
        case r180:
            if (mino->type == I) {
                mino->y += g_irotation_lut[mino->dir].dy;
                mino->x += g_irotation_lut[mino->dir].dx;
                mino->y += g_irotation_lut[(mino->dir + 1) % 4].dy;
                mino->x += g_irotation_lut[(mino->dir + 1) % 4].dx;
                mino->dir = (mino->dir + 2) % 4;
                break;
            }
            mino->dir = (mino->dir + 2) % 4;
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
        case r270:         /* "-(pi/2)", aka 90° clockwise */
            v->dy = -px;
            v->dx = py;
            break;
        case r90:          /* "(pi/2)", aka 90° anti-clockwise */
            v->dy = px;
            v->dx = -py;
            break;
        case r180:         /* "2(pi/2)", aka 180° */
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
