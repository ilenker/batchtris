#include <ncurses.h>
#include <stdlib.h>
#include "minofunc.h"

mino_t *make_mino(char type) {
    mino_t *new_mino = malloc(sizeof(mino_t));
    if (new_mino == NULL) {
        return NULL;
    }
    new_mino->type = type;
    new_mino->rot = 0;
    new_mino->x = 8;
    new_mino->y = 8;
    switch (type) {
        case 'I':
            new_mino->v[0].dy = 0; 
            new_mino->v[0].dx = -1; 
            new_mino->v[1].dy = 0;          
            new_mino->v[1].dx = 1;       // oxoo
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 2; 
            break;
        case 'O':
            new_mino->v[0].dy = -1;      //  oo 
            new_mino->v[0].dx = 0;      //   xo
            new_mino->v[1].dy = -1; 
            new_mino->v[1].dx = 1; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case 'J':
            new_mino->v[0].dy = -1;     // o
            new_mino->v[0].dx = -1;    //  oxo
            new_mino->v[1].dy = 0; 
            new_mino->v[1].dx = -1; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case 'L':
            new_mino->v[0].dy = 0;      //   o
            new_mino->v[0].dx = -1;    //  oxo
            new_mino->v[1].dy = 0; 
            new_mino->v[1].dx = 1; 
            new_mino->v[2].dy = -1; 
            new_mino->v[2].dx = 1; 
            break;
        case 'S':
            new_mino->v[0].dy = 0; 
            new_mino->v[0].dx = -1;     //  oo
            new_mino->v[1].dy = -1;    //  ox
            new_mino->v[1].dx = 0; 
            new_mino->v[2].dy = -1; 
            new_mino->v[2].dx = 1; 
            break;
        case 'Z':
            new_mino->v[0].dy = -1; 
            new_mino->v[0].dx = -1;     // oo
            new_mino->v[1].dy = -1;    //   xo
            new_mino->v[1].dx = 0; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 1; 
            break;
        case 'T':
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

char get_mino_color(char type) {
    switch (type) {
        case 'I':
            return 1;
        case 'O':
            return 2;
        case 'J':
            return 3;
        case 'L':
            return 4;
        case 'S':
            return 5;
        case 'Z':
            return 6;
        case 'T':
            return 7;
        default:
            return 8;
    }
}

void render_mino(mino_t *mino, char ch) {
    attron(COLOR_PAIR(9));
    mvprintw(1, 3, "rot: %d", mino->rot);
    mvprintw(2, 3, "[0]x: %d | [0]y: %d", mino->v[0].dx, mino->v[0].dy);
    mvprintw(3, 3, "[1]x: %d | [1]y: %d", mino->v[1].dx, mino->v[1].dy);
    mvprintw(4, 3, "[2]x: %d | [2]y: %d", mino->v[2].dx, mino->v[2].dy);
    char col = ch == '0' ? 8 : get_mino_color(mino->type);
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

void rotate_mino(mino_t *mino, char dir) {         /*        flip x and y         */

//  for (char i = 0; i < 3; i++) {      /* If not even, also flip sign */ 
//      mino->v[i].dy = mino->v[i].dy ^ mino->v[i].dx; 
//      mino->v[i].dx = mino->v[i].dy ^ mino->v[i].dx; 
//      mino->v[i].dy = mino->v[i].dy ^ mino->v[i].dx; 

//      if (mino->rot % 2 != 0) {      
//          mino->v[i].dy = 1 + (mino->v[i].dy ^ 255);
//          mino->v[i].dx = 1 + (mino->v[i].dx ^ 255);
//      }
//  }
    mino->rot = mino->rot < 3 ? mino->rot + 1 : 0;
    rotate_vector(&mino->v[0], dir);
    rotate_vector(&mino->v[0], dir);
    rotate_vector(&mino->v[0], dir);

    rotate_vector(&mino->v[1], dir);
    rotate_vector(&mino->v[1], dir);
    rotate_vector(&mino->v[1], dir);

    rotate_vector(&mino->v[2], dir);
    rotate_vector(&mino->v[2], dir);
    rotate_vector(&mino->v[2], dir);
}


void rotate_vector(vec_t *v, char dir) {
    char px = v->dx;
    char py = v->dy;
    switch (dir) {
        case -1:
            v->dx = py;
            v->dy = -px;
            break;
        case 1:
            v->dx = -py;
            v->dy = px;
            break;
        case 2:
            v->dx = -px;
            v->dy = -py;
            break;
    }
}
void rotate_vector_BKP(vec_t *v, char dir) {
    switch (dir) {
        case 2:
            v->dx = (v->dx * 0) + (v->dy * 1);
            v->dy = -1 * (v->dx * 1) + (v->dy * 0);
            break;
        case 1:
            v->dx = (v->dx * 0) + (v->dy * -1);
            v->dy = -1 * (v->dx * -1) + (v->dy * 0);
            break;
        case -1:
            v->dx = (v->dx * -1) + (v->dy * 0);
            v->dy = -1 * (v->dx * 0) + (v->dy * -1);
            break;
    }
}

void test_minos() {
    mino_t *I = make_mino('I');
    I->x = 1;
    I->y = 5;
    render_mino(I, '1');

    mino_t *O = make_mino('O');
    O->x = 10;
    O->y = 5;
    render_mino(O, '1');

    mino_t *J = make_mino('J');
    J->x = 15;
    J->y = 5;
    render_mino(J, '1');

    mino_t *L = make_mino('L');
    L->x = 5;
    L->y = 10;
    render_mino(L, '1');

    mino_t *S = make_mino('S');
    S->x = 10;
    S->y = 10;
    render_mino(S, '1');

    mino_t *Z = make_mino('Z');
    Z->x = 15;
    Z->y = 10;
    render_mino(Z, '1');

    mino_t *T = make_mino('T');
    T->x = 15;
    T->y = 15;
    render_mino(T, '1');
    free(I);
    free(O);
    free(J);
    free(L);
    free(S);
    free(Z);
    free(T);
}
