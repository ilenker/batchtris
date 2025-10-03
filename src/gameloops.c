#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"
#include "gameloops.h"
#include "init_think_execute.h"

// general init 
game_state_t GAME_STATE;
int state_update;
char input;

bool hold_available;
bool first_hold;
shape_t hold;
int g_gravity_timer;

motion_t input_sequence[64];
int seq_index;
int seq_len;
int piece_count;

char *mn[8];
char g_debug_verbosity;
sprite_t sprite_data[16];
row_t *row_iterator_index;

void game_variables_init() {
    // General
    GAME_STATE = MENU;
    hold_available = true;
    first_hold = true;

    // Classic Specific
    g_gravity_timer = 500;
    
    // !think! !execute! Specific
    seq_index = 0; 
    seq_len = 0; 
    piece_count = 0;

    mn[0] = "ROTATE_CW";
    mn[1] = "ROTATE_CCW";
    mn[2] = "ROTATE_180";
    mn[3] = "MOVE_LEFT";
    mn[4] = "MOVE_RIGHT";
    mn[5] = "HARD_DROP";
    mn[6] = "SOFT_DROP";
    mn[7] = "HOLD";

    // Other
    g_debug_verbosity = 0;
}

            /* qwfpmode */
game_state_t mode_select() {
    static unsigned int select = 0;
    mvprintw(12, 5 + 5, "!think!");
    mvprintw(14, 5, "block stacker game");

    while (input != ' ') {
        input = getch();
        if (input == 'e') {
            select = (select - 1) % 2;
        }
        if (input == 'n') {
            select = (select + 1) % 2;
        }
        if (select == 0) {
            standout();
            mvprintw(12, 5 + 5, "!think!");
            standend();
            mvprintw(14, 5, "block stacker game");
        }
        if (select == 1) {
            mvprintw(12, 5 + 5, "!think!");
            standout();
            mvprintw(14, 5, "block stacker game");
            standend();
        }
    }
    if (select == 0) {
        return INPUT_MOVES;
    }
    if (select == 1) {
        return CLASSIC_TETRIS;
    }
    return MENU;
}

            /* qwfpclassic */
game_state_t classic_tetris() {
        input = getch();
        switch (input) {                    
            case 'R':                          
            case 'r':                          
                mino_render('0');
                mino_resolve_motion(ROTATE_CCW);
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case 'S':                           
            case 's':                           
                mino_render('0');
                mino_resolve_motion(ROTATE_CW);
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case 'T':                            
            case 't':                            
                mino_render('0');
                mino_resolve_motion(ROTATE_180);
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case 'H':                             
            case 'h':                             
                mino_render('0');
                mino_resolve_motion(MOVE_LEFT);
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case ':':
                mino_render('0');
                mino_resolve_motion(MOVE_RIGHT);
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case ' ':
                //first_seq->active = true;
                mino_render('0');
                state_update = mino_resolve_motion(HARD_DROP);
                hold_available = true;
                mino_render('1');
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                wnoutrefresh(board_win);
                break;
            case 'Z':
            case 'z':
                mino_render('0');
                if (mino_resolve_motion(SOFT_DROP) == 3) {
                    wnoutrefresh(board_win);
                }
                mino_render('1');
                wnoutrefresh(board_win);
                break;
            case 'A':
            case 'a':
                if (hold_available) {
                    mino_render('0');
                    if (first_hold) {
                        hold = mino->type;
                        mino_reset(bag_next());
                        first_hold = false;
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                    } else {
                        hold ^= mino->type;
                        mino->type ^= hold;
                        hold ^= mino->type;
                        mino_reset(mino->type);
                    }
                    sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                    hold_available = false;
                    mino_render('1');
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
                    row_iterator_index = row_iterator(board->head, 0); // sll iteration
                    if (row_iterator_index == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (row_iterator_index != NULL) {
                            printf("%d", row_iterator_index->data[j]);
                        }
                    }
                    printf("\n");
                }
                //free(first_seq);
                free(mino);
                free(board);
                return 0;
            case 'm':
                GAME_STATE = MENU;
                return MENU;
                break;
            case '1':
                first_hold = true;
                break;
        }
        if (g_gravity_timer < 0) {    
            g_gravity_timer = 500;
            mino_render('0');
            mino_resolve_motion(GRAVITY);
            mino_render('1');
            wnoutrefresh(board_win);
        }

        //if (first_seq->active && timer < 32) {
        //    if (first_seq->frame[timer]->type != event_null) {
        //        event_t *f = first_seq->frame[timer];
        //        f->event(stdscr, (char*)f->content, f->y, f->x); 
        //    }
        //    if (g_gravity_timer % 30 == 0) {
        //        timer++;
        //    }
        //}  
        debug_display(g_debug_verbosity);
        g_gravity_timer -= 10;
        if (state_update != SUCCESS_UPDATE) doupdate();
        state_update = BETS_ARE_OFF;
        napms(15);
    return CLASSIC_TETRIS;
}

            /* qwfpinput */
game_state_t input_moves() {
        static int _foo = 4;
        static int x_limit = 0;
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
                if (x_limit > -8) {
                    input_sequence[seq_index] = MOVE_LEFT;
                    seq_index++;
                    x_limit--;
                }
                break;
            case ':':
                if (x_limit < 8) {
                    input_sequence[seq_index] = MOVE_RIGHT;
                    seq_index++;
                    x_limit++;
                }
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
                    mino_render('0');
                    if (first_hold) {
                        hold = mino->type;
                        mino_reset(bag_next());
                        first_hold = false;
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                        sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                    } else {
                        hold ^= mino->type;
                        mino->type ^= hold;
                        hold ^= mino->type;
                        mino_reset(mino->type);
                    }
                    sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                    hold_available = false;
                    x_limit = 0;
                    mino_render('1');
                    wnoutrefresh(board_win);
                } else {
                    // flash hold mino
                }
                break;
            case ' ':
                input_sequence[seq_index] = HARD_DROP;
                mvprintw(BOARD_Y + _foo,  BOARD_X + 22, "_____");
                _foo+=3;
                x_limit = 0;
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
                    row_iterator_index = row_iterator(board->head, 0); // sll iteration
                    if (row_iterator_index == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        printf("%d", row_iterator_index->data[j]);
                    }
                    printf("\n");
                }
                //free(first_seq);
                free(mino);
                free(board);
                return 0;
            case '1':
                break;
            default:
                break;
        }
            if (seq_index > 0 && (input_sequence[seq_index - 1] == MOVE_LEFT ||
                                  input_sequence[seq_index - 1] == MOVE_RIGHT  )) {
                mvwprintw(input_move_window, seq_index - x_limit, 1,
                          "%d: %s", seq_index, mn[input_sequence[seq_index - 1]]);
            } else {
            mvwprintw(input_move_window, seq_index, 1,
                      "%d: %s", seq_index, mn[input_sequence[seq_index - 1]]);
        }

        wnoutrefresh(input_move_window);
        wnoutrefresh(board_win);
        debug_display(g_debug_verbosity);
        if (piece_count == 4) {
            mvprintw(BOARD_Y + _foo,  BOARD_X + 22, "_____");
            mvprintw(BOARD_Y - 1, BOARD_X + 6, "!execute!");
            wnoutrefresh(board_win);
            wnoutrefresh(stdscr);
            nodelay(board_win, true);
            nodelay(stdscr, true);
            doupdate();
            seq_len = seq_index ;
            seq_index = 0;
            GAME_STATE = EXECUTE_MOVES;
            return EXECUTE_MOVES;
        }
        doupdate();
    return INPUT_MOVES;
}

            /* qwfpexecute */
game_state_t execute_moves() {
    static int _i = 0;
    input = input_sequence[seq_index];            
    mvwprintw(execute_move_window, 1 + _i++, 1, "%d: %s", _i + 1, mn[input_sequence[seq_index]]);
    wnoutrefresh(execute_move_window);
    doupdate();
    napms(150);
    switch (input) {                    
        case ROTATE_CCW:                          
            mino_render('0');
            mino_resolve_motion(ROTATE_CCW);
            mino_render('1');
            wnoutrefresh(board_win);
            seq_index++;
            break;
        case ROTATE_CW:                           
            mino_render('0');
            mino_resolve_motion(ROTATE_CW);
            mino_render('1');
            wnoutrefresh(board_win);
            seq_index++;
            break;
        case ROTATE_180:                            
            mino_render('0');
            mino_resolve_motion(ROTATE_180);
            mino_render('1');
            wnoutrefresh(board_win);
            seq_index++;
            break;
        case MOVE_LEFT:                             
            mino_render('0');
            mino_resolve_motion(MOVE_LEFT);
            mino_render('1');
            wnoutrefresh(board_win);
            doupdate();
            napms(100);
            seq_index++;
            while (input_sequence[seq_index] == MOVE_LEFT) {
                mino_render('0');
                mino_resolve_motion(MOVE_LEFT);
                mino_render('1');
                wnoutrefresh(board_win);
                doupdate();
                napms(1);
                seq_index++;
            }
            mino_render('1');
            wnoutrefresh(board_win);
            break;
        case MOVE_RIGHT:
            mino_render('0');
            mino_resolve_motion(MOVE_RIGHT);
            mino_render('1');
            wnoutrefresh(board_win);
            doupdate();
            napms(100);
            seq_index++;
            while (input_sequence[seq_index] == MOVE_RIGHT) {
                mino_render('0');
                mino_resolve_motion(MOVE_RIGHT);
                mino_render('1');
                wnoutrefresh(board_win);
                doupdate();
                napms(1);
                seq_index++;
            }
            mino_render('1');
            wnoutrefresh(board_win);
            break;
        case HARD_DROP:
            mino_render('0');
            state_update = mino_resolve_motion(HARD_DROP);
            hold_available = true;
            mino_render('1');
            //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
            //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
            //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
            //sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
            wnoutrefresh(board_win);
            seq_index++;
            break;
        case SOFT_DROP:
            mino_render('0');
            if (mino_resolve_motion(SOFT_DROP) == 3) {
                wnoutrefresh(board_win);
            }
            mino_render('1');
            wnoutrefresh(board_win);
            seq_index++;
            break;
        case HOLD:
            // TODO: fix hold
            if (hold_available) {
                mino_render('0');
                if (first_hold) {
                    hold = mino->type;
                    mino_reset(bag_next());
                    first_hold = false;
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]], BOARD_Y + 5,  BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]], BOARD_Y + 8,  BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]], BOARD_Y + 11, BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]], BOARD_Y + 14, BOARD_X + 21);
                } else {
                    hold ^= mino->type;
                    mino->type ^= hold;
                    hold ^= mino->type;
                    mino_reset(mino->type);
                }
                sprite_draw_yx(sprites, &sprite_data[hold], 7, 1);
                hold_available = false;
                mino_render('1');
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
                    row_iterator_index = row_iterator(board->head, 0); // sll iteration
                    if (row_iterator_index == NULL) {break;}
                    printf("%2d: ", i);
                    for (int j = 0; j < 10; j++) {
                        if (row_iterator_index != NULL) {
                            printf("%d", row_iterator_index->data[j]);
                        }
                    }
                    printf("\n");
                }
                //free(first_seq);
                free(mino);
                free(board);
                return 0;
            case '1':
                first_hold = true;
                break;
            default:
                break;
        }
        if (seq_index >= seq_len) {
            GAME_STATE = INPUT_MOVES;
            seq_index = 0;
            mvprintw(BOARD_Y - 1, BOARD_X + 6, " !think! ");
            mino_render('0');
            werase(input_move_window);
            doupdate();
            return INPUT_MOVES;
        }
    return EXECUTE_MOVES;
}

void ui_make_message(WINDOW *window , char *msg, int y, int x) {
    mvwaddstr(window, y, x, msg);
}

