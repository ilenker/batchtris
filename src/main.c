#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"
#include "gameui.h"
#include <time.h>

/*    (^=__=^)
 *    ~Overview~                      
 *                                    [complexity][time]  
 *       
 *  FUNDAMENTAL MECHANICS    
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
#define COLOR_ORANGE 9
#define COLOR_PURPLE 10
#define COLOR_GREY 11

int main() {
    bool itiswhatitis = true;
    bool hold_available = true;
    bool first_hold = true;
    game_state_t GAME_STATE = INPUT_MOVES;
    shape_t hold;
    char g_debug_verbosity = 0;
    int g_gravity_timer = 500;
    time_t t;
    srand(time(&t));

    //___________INIT___________//
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, 0);
    nodelay(stdscr, 1);
    leaveok(stdscr, 1);
    curs_set(0);

    // TODO: Rework color structure (don't use pairs for everything)
    // TODO: support 8 color terminals
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
    init_pair(11, COLOR_GREY, COLOR_BLACK); // Text 
    wbkgd(stdscr, COLOR_PAIR(10));

           /* BOARD WINDOW INIT */
    WINDOW *board_win = newwin(20, 20, BOARD_Y, BOARD_X); // 20x20 board at y=6, x=6  
    scrollok(board_win, 0);
    leaveok(board_win, 1);
    nodelay(board_win, 1);
    wclear(stdscr);
    wbkgd(board_win, COLOR_PAIR(11) | '_');
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
    //mino_render(board_win, mino, '1');
    board_render(board, board_win);

         /* SPRITE PAD INIT */
    WINDOW *sprites = newpad(16, 16); 
    sprite_t sprite_data[16];
    sprites_minos_init(sprites, &sprite_data[0]);

         /* More windows testing here rework later yes */
    WINDOW *input_move_window = newwin(20, 16, BOARD_Y, BOARD_X + 30); // 20x20 board at y=6, x=6  
    wattron(input_move_window, COLOR_PAIR(9));
    scrollok(input_move_window , 0);
    leaveok(input_move_window , 1);
    nodelay(input_move_window , 1);
    WINDOW *execute_move_window = newwin(20, 16, BOARD_Y, BOARD_X - 17); // 20x20 board at y=6, x=6  
    wattron(execute_move_window, COLOR_PAIR(9));
    scrollok(execute_move_window , 0);
    leaveok(execute_move_window , 1);
    nodelay(execute_move_window , 1);

    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
    mvprintw(BOARD_Y + 19, BOARD_X, "L_L_L_L_L_L_L_L_L_L_");

    wnoutrefresh(board_win);

                    sequence_t *first_seq = calloc(1, sizeof(sequence_t));
                    event_t ev1 = {event_message, ui_make_message, "message one letsgoo", 8, 50, 0};
/* wip eventcode */ event_t ev2 = {event_message, ui_make_message, "anotha one", 11, 48, 0};
                    event_t evnull = {event_null, NULL, NULL, 0, 0, 0};
                    for (int i = 0; i < 32; i++) {
                        first_seq->frame[i] = &evnull;
                    }
                    first_seq->frame[0] = &ev1;
                    first_seq->frame[20] = &ev2;
                    first_seq->active = false;
                    int timer = 0;

    char input;
    int state_update;
    while (GAME_STATE == CLASSIC_TETRIS) {
        input = getch();            
        switch (input) {                    
            case 'R':                          
            case 'r':                          
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CCW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'S':                           
            case 's':                           
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'T':                            
            case 't':                            
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_180);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'H':                             
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
                first_seq->active = true;
                mino_render(board_win, mino, '0');
                state_update = mino_resolve_motion(board, mino, HARD_DROP);
                hold_available = true;
                mino_render(board_win, mino, '1');
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                wnoutrefresh(board_win);
                break;
            case 'Z':
            case 'z':
                mino_render(board_win, mino, '0');
                if (mino_resolve_motion(board, mino, SOFT_DROP) == 3) {
                    wnoutrefresh(board_win);
                }
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case 'A':
            case 'a':
                if (hold_available) {
                    mino_render(board_win, mino, '0');
                    if (first_hold) {
                        hold = mino->type;
                        mino_reset(mino, bag_next(board));
                        first_hold = false;
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                    } else {
                        hold ^= mino->type;
                        mino->type ^= hold;
                        hold ^= mino->type;
                        mino_reset(mino, mino->type);
                    }
                    sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                    hold_available = false;
                    mino_render(board_win, mino, '1');
                    wnoutrefresh(board_win);
                } else {
                    // flash hold mino
                }
                break;
            case 'q':
                delwin(board_win);
                endwin();
                printf("\n");
                printf("----------debug------------\n");
                
                row_iterator(NULL, 1);
                for (int i = 0; i < 20; i++) {
                    current_row = row_iterator(board->head, 0); // sll iteration
                    if (current_row == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (current_row != NULL) {
                            printf("%d", current_row->data[j]);
                        }
                    }
                    printf("\n");
                }
                free(first_seq);
                free(mino);
                free(board);
                return 0;
            case '1':
                first_hold = true;
                main(); // totally professional clean 100x vibes are on.
                break;
        }
        if (g_gravity_timer < 0) {    
            g_gravity_timer = 500;
            mino_render(board_win, mino, '0');
            mino_resolve_motion(board, mino, GRAVITY);
            mino_render(board_win, mino, '1');
            wnoutrefresh(board_win);
        }


        if (first_seq->active && timer < 32) {
            if (first_seq->frame[timer]->type != event_null) {
                event_t *f = first_seq->frame[timer];
                f->event(stdscr, (char*)f->content, f->y, f->x); 
            }
            if (g_gravity_timer % 30 == 0) {
                timer++;
            }
        }  
        debug_display(mino, board, g_debug_verbosity);
        g_gravity_timer -= 10;
        if (state_update != SUCCESS_UPDATE) doupdate();
        state_update = BETS_ARE_OFF;
        napms(8);
    }

    char *mn[8];
    mn[0] = "ROTATE_CW";
    mn[1] = "ROTATE_CCW";
    mn[2] = "ROTATE_180";
    mn[3] = "MOVE_LEFT";
    mn[4] = "MOVE_RIGHT";
    mn[5] = "HARD_DROP";
    mn[6] = "SOFT_DROP";
    mn[7] = "HOLD";
    zimbabwe: ; 
    motion_t input_sequence[64];
    int seq_index = 0; 
    int seq_len = 0; 
    int piece_count = 0;
    nodelay(board_win, false);
    nodelay(stdscr, false);
    mvprintw(BOARD_Y - 1, BOARD_X + 6, " !think! ");
    wnoutrefresh(board_win);
    wnoutrefresh(input_move_window);
    doupdate();
    int _foo = 4;
    mvprintw(BOARD_Y + _foo,  BOARD_X + 22, "  \\/");
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 0) % 14]], BOARD_Y + 5,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 8,  BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 11, BOARD_X + 21);
    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 14, BOARD_X + 21);
    while (GAME_STATE == INPUT_MOVES) {
        input = getch();            
        switch (input) {                    
            case 'R':                          
            case 'r':                          
                input_sequence[seq_index] = ROTATE_CCW;
                seq_index++;
                break;
            case 'S':                           
            case 's':                           
                input_sequence[seq_index] = ROTATE_CW;
                seq_index++;
                break;
            case 'T':                            
            case 't':                            
                input_sequence[seq_index] = ROTATE_180;
                seq_index++;
                break;
            case 'H':                             
            case 'h':                             
                input_sequence[seq_index] = MOVE_LEFT;
                seq_index++;
                break;
            case ':':
                input_sequence[seq_index] = MOVE_RIGHT;
                seq_index++;
                break;
            case 'Z':
            case 'z':
                input_sequence[seq_index] = SOFT_DROP;
                seq_index++;
                break;
            case 'A':
            case 'a':
                input_sequence[seq_index] = HOLD;
                seq_index++;
                if (hold_available) {
                    mino_render(board_win, mino, '0');
                    if (first_hold) {
                        hold = mino->type;
                        mino_reset(mino, bag_next(board));
                        first_hold = false;
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                    } else {
                        hold ^= mino->type;
                        mino->type ^= hold;
                        hold ^= mino->type;
                        mino_reset(mino, mino->type);
                    }
                    sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                    hold_available = false;
                    mino_render(board_win, mino, '1');
                    wnoutrefresh(board_win);
                } else {
                    // flash hold mino
                }
                break;
            case ' ':
                input_sequence[seq_index] = HARD_DROP;
                mvprintw(BOARD_Y + _foo,  BOARD_X + 22, "_____");
                _foo+=3;
                mvprintw(BOARD_Y + _foo,  BOARD_X + 22, "  \\/");
                wnoutrefresh(input_move_window);
                hold_available = true;
                seq_index++;
                piece_count++;
                break;
            case 'q':
                delwin(board_win);
                endwin();
                printf("\n");
                printf("----------debug------------\n");
                row_iterator(NULL, 1);
                for (int i = 0; i < 20; i++) {
                    current_row = row_iterator(board->head, 0); // sll iteration
                    if (current_row == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        printf("%d", current_row->data[j]);
                    }
                    printf("\n");
                }
                free(first_seq);
                free(mino);
                free(board);
                return 0;
            case '1':
                main(); // totally professional clean 100x vibes are on.
                break;
            default:
                break;
        }
        mvwprintw(input_move_window, seq_index, 1, "%d: %s", seq_index, mn[input_sequence[seq_index-1]]);
        wnoutrefresh(input_move_window);
        wnoutrefresh(board_win);
        debug_display(mino, board, g_debug_verbosity);
        if (piece_count == 4) {
            mvprintw(BOARD_Y - 1, BOARD_X + 6, "!execute!");
            wnoutrefresh(board_win);
            wnoutrefresh(stdscr);
            nodelay(board_win, true);
            nodelay(stdscr, true);
            doupdate();
            seq_len = seq_index ;
            seq_index = 0;
            GAME_STATE = EXECUTE_MOVES;
            break;
        }
        doupdate();
    }

    werase(execute_move_window);
    int _i = 0;
    while (GAME_STATE == EXECUTE_MOVES) {
        input = input_sequence[seq_index];            
        mvwprintw(execute_move_window, 1 + _i++, 1, "%d: %s", _i + 1, mn[input_sequence[seq_index]]);
        wnoutrefresh(execute_move_window);
        doupdate();
        napms(150);
        switch (input) {                    
            case ROTATE_CCW:                          
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CCW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                seq_index++;
                break;
            case ROTATE_CW:                           
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_CW);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                seq_index++;
                break;
            case ROTATE_180:                            
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, ROTATE_180);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                seq_index++;
                break;
            case MOVE_LEFT:                             
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, MOVE_LEFT);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                doupdate();
                napms(100);
                seq_index++;
                while (input_sequence[seq_index] == MOVE_LEFT) {
                    mino_render(board_win, mino, '0');
                    mino_resolve_motion(board, mino, MOVE_LEFT);
                    mino_render(board_win, mino, '1');
                    wnoutrefresh(board_win);
                    doupdate();
                    napms(1);
                    seq_index++;
                }
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case MOVE_RIGHT:
                mino_render(board_win, mino, '0');
                mino_resolve_motion(board, mino, MOVE_RIGHT);
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                doupdate();
                napms(100);
                seq_index++;
                while (input_sequence[seq_index] == MOVE_RIGHT) {
                    mino_render(board_win, mino, '0');
                    mino_resolve_motion(board, mino, MOVE_RIGHT);
                    mino_render(board_win, mino, '1');
                    wnoutrefresh(board_win);
                    doupdate();
                    napms(1);
                    seq_index++;
                }
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                break;
            case HARD_DROP:
                mino_render(board_win, mino, '0');
                state_update = mino_resolve_motion(board, mino, HARD_DROP);
                hold_available = true;
                mino_render(board_win, mino, '1');
                //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                wnoutrefresh(board_win);
                seq_index++;
                break;
            case SOFT_DROP:
                mino_render(board_win, mino, '0');
                if (mino_resolve_motion(board, mino, SOFT_DROP) == 3) {
                    wnoutrefresh(board_win);
                }
                mino_render(board_win, mino, '1');
                wnoutrefresh(board_win);
                seq_index++;
                break;
            case HOLD:
                // TODO: fix hold
                if (hold_available) {
                    mino_render(board_win, mino, '0');
                    if (first_hold) {
                        hold = mino->type;
                        mino_reset(mino, bag_next(board));
                        first_hold = false;
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                    } else {
                        hold ^= mino->type;
                        mino->type ^= hold;
                        hold ^= mino->type;
                        mino_reset(mino, mino->type);
                    }
                    sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                    hold_available = false;
                    mino_render(board_win, mino, '1');
                    wnoutrefresh(board_win);
                } else {
                    // flash hold mino
                }
                break;
            case 'q':
                delwin(board_win);
                endwin();
                printf("\n");
                printf("----------debug------------\n");
                
                row_iterator(NULL, 1);
                for (int i = 0; i < 20; i++) {
                    current_row = row_iterator(board->head, 0); // sll iteration
                    if (current_row == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (current_row != NULL) {
                            printf("%d", current_row->data[j]);
                        }
                    }
                    printf("\n");
                }
                free(first_seq);
                free(mino);
                free(board);
                return 0;
            case '1':
                first_hold = true;
                main(); // totally professional clean 100x vibes are on.
                break;
            default:
                break;
        }
        if (seq_index >= seq_len) {
            GAME_STATE = INPUT_MOVES;
            seq_index = 0;
            mvprintw(BOARD_Y - 1, BOARD_X + 6, " !think! ");
            mino_render(board_win, mino, '0');
            werase(input_move_window);
            doupdate();
            goto zimbabwe;
        }
        //debug_display(mino, board, g_debug_verbosity);
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
