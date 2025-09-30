#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"

/*     
 *    
 *    (^=__=^)
 *    ~Overview~                      [complexity][time]
 *       
 *  FUNDAMENTAL MECHANICS    
 *     |_piece hold                   [v.easy]    [quick]
 *     |_scoring                      [medium]    [mid]
 *     \_combos and B2Bs              [medium]    [mid]
 *
 *  PLANTRIS MECHANICS
 *     |_plan phase:
 *     |     |_text interface
 *     |     |_based on finesse
 *     |     |_need rapid prototypes
 *     |     |_
 *     |_execute phase: uses
 *       fundamentals, replay
 *       a sequence of inputs
 *
 *  MENU SYSTEM
 *     |_board/window states          [hard?]     [long] 
 *     |_used for tutorials,          [indeed]    [yes]
 *     |   plan-execute loop         
 *     \_transitions                  [medium]    [long]
 *
 *  AESTHETICS
 *     |_little window for
 *     | combo/clear display
 *     | w/sick animations
 *     | when you TSD
 *     \_way to do nice easeing
 *       on screen transitions
 *  
 *  PROBABLYS
 *     \_40l sprint
 *                                    
 *  MAYBES                            
 *     |_live undo                    [hard]      [long]
 *     |_finesse logic                [hard]      [long]
 *     \_customize themes             [maybe]     [mid]
 *                                    
 *  PROBABLY NOTS                     
 *     |_solvers(find t-spin          [ultra]     [forever]
 *     | setups? doubles? efficiency?)
 *     \_xinput mode for key          [basically impossible]
 *         release, DAS, ARR
 *
 */

#define SUCCESS_NOUPDATE 0
#define SUCCESS_UPDATE 2
#define FAIL_NOUPDATE 3
#define BETS_ARE_OFF -1

bool itiswhatitis = true;
char COLOR_ORANGE = 9;
char COLOR_PURPLE = 10;
char COLOR_GREY = 11;
char g_debug_verbosity = 1;
int g_gravity_timer = 500;

int main() {
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, 0);
    nodelay(stdscr, 1);
    leaveok(stdscr, 1);
    curs_set(0);

    // TODO: Rework color structure (don't use pairs for everything)
    start_color();
    init_color(COLOR_ORANGE, 929, 500, 100);
    init_color(COLOR_PURPLE, 650, 20, 900);
    init_color(COLOR_GREY, 130, 130, 130);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);            // I
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);       // O 
    init_pair(3, COLOR_BLUE, COLOR_BLUE);          // J
    init_pair(4, COLOR_ORANGE, COLOR_ORANGE);     // L
    init_pair(5, COLOR_GREEN, COLOR_GREEN);      // S
    init_pair(6, COLOR_RED, COLOR_RED);         // Z
    init_pair(7, COLOR_PURPLE, COLOR_PURPLE);  // T
    init_pair(8, COLOR_BLACK, COLOR_BLACK);   // Blank 
    init_pair(9, COLOR_WHITE, COLOR_BLACK);  // Text 
    init_pair(10, COLOR_WHITE, COLOR_GREY); // Text 
    wbkgd(stdscr, COLOR_PAIR(10));

           /* BOARD WINDOW INIT */
    WINDOW *board_win = newwin(20, 20, BOARD_Y, BOARD_X); // 20x20 board at y=6, x=6  
    scrollok(board_win, 0);
    leaveok(board_win, 1);
    nodelay(board_win, 1);
    wclear(stdscr);
    wbkgd(board_win, COLOR_PAIR(8) | '*');
    refresh();

    board_t *board = calloc(1, sizeof(board_t));
    board->parent_window = board_win;
    board_init_sll(board);
    row_t *current_row;
    row_iterator(NULL, 1);
    for (int y = 0; y < 20; y++) {
        current_row = row_iterator(board->head, 0); 
        if (current_row == NULL) {
            break;
        } 
        current_row->count = 0;
    }
    row_iterator(NULL, 1);  
    board_init(board, 20, 10);

            /* MINO INIT */
    mino_t *mino = mino_init(board->bag[0]);
    mino_render(board_win, mino, '1');
    board_render(board, board_win);

         /* SPRITE PAD INIT */
    WINDOW *sprites = newpad(16, 16); 
    sprite_t sprite_data[16];
    sprites_minos_init(sprites, &sprite_data[0]);

    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], 11, 27);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], 14, 27);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], 17, 27);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], 20, 27);

    wnoutrefresh(board_win);

                    char arstg;
    char input;
    int state_update;
    while (itiswhatitis) {
        input = getch();            
        switch (input) {                    
            case 'r':                          
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CCW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 's':                           
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 't':                            
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_180);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'h':                             
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, MOVE_LEFT);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case ':':
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, MOVE_RIGHT);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case ' ':
                mino_render(board_win, mino, '0');
                state_update = mino_resolve_motion(board, mino, HARD_DROP);
                mino_render(board_win, mino, '1');

    switch (board->bag[board->bag_index]) {
                    case 1: arstg = 'I'; break;
                    case 2: arstg = 'O'; break;
                    case 3: arstg = 'J'; break;
                    case 4: arstg = 'L'; break;
                    case 5: arstg = 'S'; break;
                    case 6: arstg = 'Z'; break;
                    case 7: arstg = 'T'; break;
    }
    mvwprintw(stdscr, 1, 5, "%c", arstg);
    mvwprintw(stdscr, 3, 5 + 14 + (board->bag_index * 3), "   ^ ");
    mvwprintw(stdscr, 2, 5, "bag index (%2d): [%d, %d, %d, %d, %d, %d, %d]", board->bag_index,
              board->bag[0],
              board->bag[1],
              board->bag[2],
              board->bag[3],
              board->bag[4],
              board->bag[5],
              board->bag[6]);
    wprintw(stdscr, "[%d, %d, %d, %d, %d, %d, %d]",
              board->bag[7],
              board->bag[8],
              board->bag[9],
              board->bag[10],
              board->bag[11],
              board->bag[12],
              board->bag[13]);
                
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], 11, 27);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], 14, 27);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], 17, 27);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], 20, 27);
                wnoutrefresh(board_win);
                break;
            case 'z':
                mino_render(board_win, mino, '0');
                if (mino_resolve_motion(board, mino, SOFT_DROP) == 3) {
                    wnoutrefresh(board_win);
                }
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'q':
                delwin(board_win);
                endwin();
                printf("\n");
                printf("----------debug------------\n");
                row_t *current_row;
                row_iterator(NULL, 1);
                for (int i = 0; i < 20; i++) {
                    current_row = row_iterator(board->head, 0); // sll iteration
                    if (current_row == NULL) {break;}
                    printf("%d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (current_row != NULL) {
                            printf("%d", current_row->data[j]);
                        }
                    }
                    printf("\n");
                }
                free(mino);
                free(board);
                return 0;
            case '1':
                main(); // totally professional clean 10x vibes are on.
        }
        if (g_gravity_timer < 0) {    
            g_gravity_timer = 500;
            mino_render(board_win, mino, '0');
            mino_resolve_motion(board, mino, GRAVITY);
            mino_render(board_win, mino, '1');
            wnoutrefresh(board_win);
        }
        debug_display(mino, board, g_debug_verbosity);
        g_gravity_timer -= 10;
        if (state_update != SUCCESS_UPDATE) doupdate();
        state_update = BETS_ARE_OFF;
        napms(17);
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
