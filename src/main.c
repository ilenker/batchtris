#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <panel.h>
#include "minofunc.h"
#include "boardfunc.h"

bool g_hatersgonnahate = true;
char COLOR_ORANGE = 9;
char COLOR_PURPLE = 10;
char COLOR_GREY = 11;
char g_debug_verbosity = 3;
int g_gravity_timer = 500;

int main() {
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, FALSE);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_color(COLOR_ORANGE, 929, 500, 100);
    init_color(COLOR_PURPLE, 650, 20, 900);
    init_color(COLOR_GREY, 200, 200, 200);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);            // I
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);       // O 
    init_pair(3, COLOR_BLUE, COLOR_BLUE);          // J
    init_pair(4, COLOR_ORANGE, COLOR_ORANGE);     // L
    init_pair(5, COLOR_GREEN, COLOR_GREEN);      // S
    init_pair(6, COLOR_RED, COLOR_RED);         // Z
    init_pair(7, COLOR_PURPLE, COLOR_PURPLE);  // T
    init_pair(8, COLOR_BLACK, COLOR_BLACK);   // Blank 
    init_pair(9, COLOR_WHITE, COLOR_BLACK);  // Text 
    init_pair(10, COLOR_ORANGE, COLOR_GREY);// Text 
    wbkgd(stdscr, COLOR_PAIR(10));

    WINDOW *board_win = newwin(20, 20, 6, 3);  // window for the board itself
    scrollok(board_win, FALSE);
    leaveok(board_win, TRUE);
    nodelay(board_win, TRUE);
    wclear(stdscr);
    wbkgd(board_win, COLOR_PAIR(8) | '*');
    refresh();

    mino_t *mino = make_mino(T);
    board_t *board = calloc(1, sizeof(board_t));
    init_board_sll(board, 20, 10);
    init_board(board, 20, 10);
    render_mino(board_win, mino, '1');
    render_board(board, board_win);

 //   endwin();
    printf("\n============TESTIN'=================\n");
/*
    // Single full row move to tail
      row_t *pre_full_row = row_at_index(board, 15-1); 
      row_t *full_row = pre_full_row->next;

      pre_full_row->next = full_row->next->next;
      full_row->next->next = board->head;
      board->head = full_row;
*/

    //row_t *tail = full_row;
    //while (tail->next != NULL) {tail = tail->next;}
    //printf("Tail: %c\n", tail->data[0]);
//head ##############
//     ##############
//     ##############
//     ##############
//     ##############
//     ##############
//     ##############
// pre ##############
//  v fXXXXXXXXXXXXXX full. node->next full? Contiguous, else just resolve the one row.
//  v  ##############
//  v fXXXXXXXXXXXXXX  after hard drop, we have updated row counts. I don't believe we necessarily 
//     ##############  start at the top. So we start at the highest (lowest index) row and go from
//     ##############  there. 
//     ##############
//tail ##############
//
    //0   
    //1
    //2
    //3
    //4
    //5
    //6
    //7
    //8
    //9
    //10
    //11
    //12
    //13
    //14      pre_full_row
                         //15  f   full_row
                         //16  f   
             //17      pre_full_new_next < Head 
             //18
             //19
    //null         count = 1

    //clear_rows(board, 15, 4);

    wnoutrefresh(board_win);

    char input;
    while (g_hatersgonnahate) {
        input = getch();
        switch (input) {
            case 'r':                          
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, ROTATE_CCW);
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 's':                           
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, ROTATE_CW);
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 't':                            
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, ROTATE_180);
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'n':                             
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, MOVE_LEFT);
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'i':
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, MOVE_RIGHT);
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case ' ':
                render_mino(board_win, mino, '0');
                resolve_mino_motion(board, mino, HARD_DROP);
                render_mino(board_win, mino, '1');
                render_board(board, board_win);
                wnoutrefresh(board_win);
                break;
            case 'e':
                render_mino(board_win, mino, '0');
                if (resolve_mino_motion(board, mino, SOFT_DROP) == 3) {
                    wnoutrefresh(board_win);
                }
                render_mino(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'q':
                delwin(board_win);
                endwin();
                printf("\n");
                printf("----------debug------------\n");
                row_t *current_row;
                yield_next_row(NULL, 1);
                for (int i = 0; i < 20; i++) {
                    current_row = yield_next_row(board->head, 0); // sll iteration
                    if (current_row == NULL) {break;}
                    printf("%d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (current_row != NULL) {
                            printf("%d", current_row->data[j]);
                        }
                    }
                    printf("\n");
                }
                printf("its ok you can do this maybe probably\n");
                free(mino);
                free(board);
                return 0;
        }
        if (g_gravity_timer < 0) {    
            g_gravity_timer = 500;
            render_mino(board_win, mino, '0');
            resolve_mino_motion(board, mino, GRAVITY);
            render_mino(board_win, mino, '1');
            wnoutrefresh(board_win);
        }
        debug_display(mino, board, g_debug_verbosity);
        g_gravity_timer -= 10;
        doupdate();
        napms(15);
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
origin delta:       0,+1        +1,0      0,-1       -1,0    

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
