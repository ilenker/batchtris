#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

bool hatersgonnahate = true;

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


void render_mino(mino_t *mino, char ch) {
    mvprintw(1, 3, "rot: %d", mino->rot);
    mvaddch(mino->y, mino->x, ch);
    for (char i = 0; i < 3; i++) {      
        mvaddch(mino->y + mino->v[i].dy,
                 mino->x + mino->v[i].dx,
                 ch);
    }
}

void rotate_mino(mino_t *mino) {         /*        flip x and y         */
    for (char i = 0; i < 3; i++) {      /* If not even, also flip sign */ 
        mino->v[i].dy = mino->v[i].dy ^ mino->v[i].dx; 
        mino->v[i].dx = mino->v[i].dy ^ mino->v[i].dx; 
        mino->v[i].dy = mino->v[i].dy ^ mino->v[i].dx; 

        if (mino->rot % 2 != 0) {      
            mino->v[i].dy = 1 + (mino->v[i].dy ^ 255);
            mino->v[i].dx = 1 + (mino->v[i].dx ^ 255);
        }
    }

    mino->rot = mino->rot < 3 ? mino->rot + 1 : 0;
}

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
            new_mino->v[1].dx = 1; 
            new_mino->v[2].dy = 0; 
            new_mino->v[2].dx = 2; 
    }
    return new_mino;
}

int main(){
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    mino_t *mino = make_mino('I');

    render_mino(mino, 'O');

    while (hatersgonnahate) {
        switch (getch()) {
            case 't':
                render_mino(mino, ' ');
                rotate_mino(mino);
                render_mino(mino, 'O');
                break;
            case 'n':
                render_mino(mino, ' ');
                mino->x--;
                render_mino(mino, 'O');
                break;
            case 'i':
                render_mino(mino, ' ');
                mino->x++;
                render_mino(mino, 'O');
                break;
            case 'c':
                free(mino);
                endwin();
                return 0;
        }
    }
}

/*
              1 2 3 4 5 6 7 8 9 A
             B 
             A
             9
             8
             7
             6
             5
             4
             3
             2
             1
empty
i o
j l
s z
t
                1234      1234       1234       1234      1234 
               a         a  O       a          a O       a     
               bOXOO     b  X       b          b O       bOXOO 
               c         c  O       cOOXO      c X       c     
               d         d  O       d          d O       d     
         dir:   right ->  down  ->   left  ->   up   ->   right
origin delta:   0,0      +1,0        0,+1      -1,0       0,-1    

Where to place blocks from origin?
   y x    y  x   y   x   y   x
r:(0,0) (0 ,-1) (0 , 1) (0 , 2)  0  
         <>      <>      <>
d:(0,0) (-1, 0) (1 , 0) (2 , 0)  1 
         <f>     <f>     <f>
l:(0,0) (0 , 1) (0 ,-1) (0 ,-2)  2 
         <>      <>      <>
u:(0,0) (1 , 0) (-1, 0) (-2, 0)  3 
         <f>     <f>     <f>
r:(0,0) (0 ,-1) (0 , 1) (0 , 2)  0 



 1 2 3 4      
a  ▒▒     0, 1
b  ><     0, 0
c  ▒▒     0,-1
d  ▒▒     0,-2
*/
