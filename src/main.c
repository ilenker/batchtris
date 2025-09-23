#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"

bool hatersgonnahate = true;
char COLOR_ORANGE = 9;
char COLOR_PURPLE = 10;

int main(){
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    start_color();
    init_color(COLOR_ORANGE, 929, 500, 100);
    init_color(COLOR_PURPLE, 650, 20, 900);

    init_pair(1, COLOR_CYAN, COLOR_CYAN);       // I
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);  //  O 
    init_pair(3, COLOR_BLUE, COLOR_BLUE);       // J
    init_pair(4, COLOR_ORANGE, COLOR_ORANGE);  //  L
    init_pair(5, COLOR_GREEN, COLOR_GREEN);     // S
    init_pair(6, COLOR_RED, COLOR_RED);        //  Z
    init_pair(7, COLOR_PURPLE, COLOR_PURPLE);   // T
    init_pair(8, COLOR_BLACK, COLOR_BLACK);    //  Blank 
    init_pair(9, COLOR_WHITE, COLOR_BLACK);    //  Text 
                                   
    mino_t *mino = make_mino('S');
    render_mino(mino, '1');

    while (hatersgonnahate) {
        switch (getch()) {
            case 'r':
                render_mino(mino, '0');
                rotate_mino(mino, 1);
                render_mino(mino, '1');
                break;
            case 's':
                render_mino(mino, '0');
                rotate_mino(mino, -1);
                render_mino(mino, '1');
                break;
            case 't':
                render_mino(mino, '0');
                rotate_mino(mino, 2);
                render_mino(mino, '1');
                break;
            case 'n':
                render_mino(mino, '0');
                mino->x--;
                render_mino(mino, '1');
                break;
            case 'i':
                render_mino(mino, '0');
                mino->x++;
                render_mino(mino, '1');
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



                1234      1234       1234       1234      1234 
               a         a          a          a         a     
               b  OO     b  O       b          bO        b  OO 
               c OX      c  XO      c  XO      cOX       c OX  
               d         d   O      d OO       d O       d     
         dir:   right ->  down  ->   left  ->   up   ->   right

Where to place blocks from origin?
   y x   y x   y x   y x
r:(0,0) (0,-) (-,0) (-,+)  0  
         - +   + +   2 0
d:(0,0) (-,0) (0,+) (+,+)  1 
         + +   + -   0 -2
l:(0,0) (0,+) (+,0) (+,-)  2 
         + -   - -  -2 0
u:(0,0) (+,0) (0,-) (-,-)  3 
         - -   - +   0 2 
r:(0,0) (0,-) (-,0) (-,+)  0  
*/
