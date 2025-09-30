#include <ncurses.h>
#include <stdlib.h>
#include "minofunc.h"
#include "boardfunc.h"

#define __break {wclear(board->parent_window); board_render(board, board->parent_window); nodelay(board->parent_window, FALSE); 

#define point__ refresh(); doupdate(); wgetch(board->parent_window); nodelay(board->parent_window, TRUE);}

const vec_t g_irotation_lut[4] = {
    {0 , 1},
    {1 , 0},
    {0 ,-1},
    {-1, 0}
};


void mino_reset(mino_t *new_mino, shape_t type) {
    new_mino->type = type;
    new_mino->dir = 0;
    new_mino->y = 2;
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
}
mino_t *mino_init(shape_t type) {
    mino_t *new_mino = calloc(1, sizeof(mino_t));
    if (new_mino == NULL) {
        return NULL;
    }
    new_mino->type = type;
    new_mino->dir = 0;
    new_mino->y = 2;
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


#define SUCCESS_NOUPDATE 0
#define SUCCESS_UPDATE 2
#define FAIL_NOUPDATE 3
#define BETS_ARE_OFF -1

int mino_resolve_motion(board_t *board, mino_t *mino, motion_t motion) {
    int peek = 0;
    switch (motion) {
        case GRAVITY:       
            if (mino->y + 1 > board->depth ||
                board_data_at_yx(board, mino->y + 1, mino->x) != 9) {
                mino->falling = false;
                return FAIL_NOUPDATE;
            }
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy + 1;
                int x_check = mino->x + mino->v[i].dx;    
                int yx_check = board_data_at_yx(board, y_check, x_check);
                if (y_check > board->depth || yx_check != 9) {
                    mino->falling = false;
                    return FAIL_NOUPDATE;
                }
            }
            mino->y++;
            return SUCCESS_NOUPDATE;
        case MOVE_LEFT:
            if (mino->x - 1 < 0 || board_data_at_yx(board, mino->y, mino->x - 1) != 9) {
                return FAIL_NOUPDATE;
            }
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx - 1;    
                if (x_check < 0 || board_data_at_yx(board, y_check, x_check)!= 9) {
                    return FAIL_NOUPDATE;
                }
            }
            mino->x--;
            return SUCCESS_NOUPDATE;
        case MOVE_RIGHT:
            if (mino->x + 1 >= board->width || board_data_at_yx(board, mino->y, mino->x + 1) != 9) {
                return FAIL_NOUPDATE;
            }
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx + 1;    
                if (x_check >= board->width || board_data_at_yx(board, y_check, x_check) != 9) {
                    return FAIL_NOUPDATE;
                }
            }
            mino->x++;
            return SUCCESS_NOUPDATE;
        // TODO: Rotation is more stable, bugs might still lurk
        case ROTATE_CW:
            mino_rotate(mino, r270);
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx;    
                if (board_data_at_yx(board, y_check, x_check) != 9) {
                    mino_rotate(mino, r90);
                    return FAIL_NOUPDATE;
                }
            }
            return SUCCESS_NOUPDATE;
        case ROTATE_CCW:
            mino_rotate(mino, r90);
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx;    
                if (board_data_at_yx(board, y_check, x_check) != 9) {
                    mino_rotate(mino, r270);
                    return FAIL_NOUPDATE;
                }
            }
            return SUCCESS_NOUPDATE;
        // TODO: Rotation is bugged I 180
        case ROTATE_180:
            mino_rotate(mino, r180);
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx;    
                if (board_data_at_yx(board, y_check, x_check) != 9) {
                    mino_rotate(mino, r180);
                    return FAIL_NOUPDATE;
                }
            }
            return SUCCESS_NOUPDATE;
        case HARD_DROP:
            while (mino_resolve_motion(board, mino, GRAVITY) != 3){} // Fall till obstacle
            char new_limit = board->render_limit; 

                           /* ===Mino origin=== */
            row_t *current_row = row_at_index(board, mino->y);
            current_row->data[mino->x] = mino->type;
            current_row->count++;
            if (mino->y < new_limit) {
                new_limit = mino->y;
            }
            if (current_row->count == 10) {  // Row is full at xy
                row_process(board, mino->y, PUSH); // ptr = 1
            }

                      /* ===Mino's 3x components=== */
            for (int i = 0; i < 3; i++) {
                int y_check = mino->y + mino->v[i].dy;
                int x_check = mino->x + mino->v[i].dx;    
                current_row = row_at_index(board, y_check);
                current_row->data[x_check] = mino->type;
                current_row->count++;

                if (current_row->count == 10) {
                    row_process(board, y_check, PUSH);
                }
                if (y_check < new_limit) {
                    new_limit = y_check;
                }
            }
            peek = row_process(board, -1, PEEK); 
            if (peek != 0) {
                new_limit -= peek;
                row_process(board, -1, CLEAR);
            }
            board->render_limit = new_limit;
            board_render(board, board->parent_window);
            mino_reset(mino, bag_next(board));
            doupdate();
            return SUCCESS_UPDATE;
        case SOFT_DROP:   
            return mino_resolve_motion(board, mino, GRAVITY);
        default:
            return BETS_ARE_OFF;
    }
}


void mino_render(WINDOW * window, mino_t *mino, char ch) {
    char col = ch == '0' ? 8 : mino->type;
    wattron(window, COLOR_PAIR(col));               /* Set the color based on mino type */
    mvwaddch(window, mino->y, mino->x * 2, ch);    /*    x*2 to stretch x res by 2     */
    waddch(window, ch);                           /* print extra ch to fill the space */

    for (int i = 0; i < 3; i++) {      
        mvwaddch(window,                          /* and do the same for each of */
                 mino->y + mino->v[i].dy,        /* minos individual components */
                (mino->x + mino->v[i].dx) * 2,
                 ch);
        waddch(window, ch);
    }
}

void mino_rotate(mino_t *mino, rot_t rot) {
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
    vector_rotate(&mino->v[0], rot);
    vector_rotate(&mino->v[0], rot);
    vector_rotate(&mino->v[0], rot);

    vector_rotate(&mino->v[1], rot);
    vector_rotate(&mino->v[1], rot);
    vector_rotate(&mino->v[1], rot);

    vector_rotate(&mino->v[2], rot);
    vector_rotate(&mino->v[2], rot);
    vector_rotate(&mino->v[2], rot);
}

void vector_rotate(vec_t *v, rot_t rot) {
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

void debug_display(mino_t *mino, board_t *board, char verbosity) {
    if (verbosity == 0) {
        return;
    }
    attron(COLOR_PAIR(9));
    switch (verbosity) {
        case 3:
            wattron(stdscr, COLOR_PAIR(9));
            row_iterator(NULL, 1);
            row_t *current_row;
            for (int y = 0; y < 20; y++) {
                current_row = row_iterator(board->head, 0); 
                if (current_row == NULL) {
                    break;
                } 
                mvprintw(6 + y, 0, "%2d:%2d", y, current_row->count);
            }
            row_iterator(NULL, 1);
            break;
        case 2:
            mvprintw(1, 3, "[0]y: %d ", mino->v[0].dy);
            mvprintw(1, 11, "| [0]x: %d ", mino->v[0].dx);
            mvprintw(2, 3, "[1]y: %d ", mino->v[1].dy);
            mvprintw(2, 11, "| [1]x: %d ", mino->v[1].dx);
            mvprintw(3, 3, "[2]y: %d ", mino->v[2].dy);
            mvprintw(3, 11, "| [2]x: %d ", mino->v[2].dx);
        break;
        case 1:
            //mvprintw(1, 3, "type: %d ", mino->type);
            mvprintw(1, 3, "limit: %d ", board->render_limit);
            mvprintw(2, 3, "dir: %d ", mino->dir);
            mvprintw(3, 3, "y: %d ", mino->y);
            mvprintw(3, 11, "| x: %d ", mino->x);
            mvprintw(4, 3, "falling: %d ", mino->falling);
        break;
    }
}
