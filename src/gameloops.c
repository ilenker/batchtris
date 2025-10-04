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
stats_t stats;

bool hold_available;
bool first_hold;
shape_t hold;
int g_gravity_timer;

game_input_t input_sequence[64];
int seq_index;
int seq_len;
int piece_count;

char *mn[9];
char g_debug_verbosity;
sprite_t sprite_data[16];
row_t *row_iterator_index;

void game_variables_init() {
    // General
    GAME_STATE = MENU;
    hold_available = true;
    first_hold = true;
    input = -1;
    stats.pc = 0;
    stats.kpp = 0.0f;
    stats.pps = 0.0f;

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
    mn[8] = "No motion :(";

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
    static int bag_cursor = 4;
    static int x_limit = 0;
    input = getch();            
    refresh();
    doupdate();
    switch (input) {                    
        case 'R':                          
        case 'r':                          
            input_sequence[seq_index].motion = ROTATE_CCW;
            seq_index++;
            break;
        case 'S':                           
        case 's':                           
            input_sequence[seq_index].motion = ROTATE_CW;
            seq_index++;
            break;
        case 'T':                            
        case 't':                            
            input_sequence[seq_index].motion = ROTATE_180;
            seq_index++;
            break;
        case 'H':                             
        case 'h':                             
            input_sequence[seq_index].motion = MOVE_LEFT;
            if (input_sequence[seq_index - 1].motion == MOVE_LEFT) {
                input_sequence[seq_index - 1].count++;
                break;
            } 
            seq_index++;
            break;
        case ':':
            input_sequence[seq_index].motion = MOVE_RIGHT;
            if (input_sequence[seq_index - 1].motion == MOVE_RIGHT) {
                input_sequence[seq_index - 1].count++;
                break;
            } 
            seq_index++;
            break;
        case 'Z':
        case 'z':
            input_sequence[seq_index].motion = SOFT_DROP;
            seq_index++;
            break;
        case 'A':
        case 'a':
            input_sequence[seq_index].motion = HOLD;
            seq_index++;
            if (hold_available) {
                mino_render('0');
                if (first_hold) {
                    hold = mino->type;
                    mino_reset(bag_next());
                    first_hold = false;
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 1) % 14]],
                                   BOARD_Y + 5,  BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 2) % 14]],
                                   BOARD_Y + 8,  BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 3) % 14]],
                                   BOARD_Y + 11, BOARD_X + 21);
                    sprite_draw_yx(sprites, &sprite_data[board->bag[(board->bag_index + 4) % 14]],
                                   BOARD_Y + 14, BOARD_X + 21);
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
            input_sequence[seq_index].motion = HARD_DROP;
            attron(COLOR_PAIR(12));
            mvprintw(BOARD_Y + bag_cursor,  BOARD_X + 22, "_____");
            bag_cursor+=3;
            x_limit = 0;
            attron(COLOR_PAIR(9));
            mvprintw(BOARD_Y + bag_cursor,  BOARD_X + 22, "  \\/");
            wnoutrefresh(input_move_window);
            hold_available = true;
            seq_index++;
            piece_count++;
            break;
        case 'q':
            endwin();
            free(mino);
            free(board);
            return 0;
        case '1':
            break;
        default:
            break;
    }
    if (seq_index > 0) {
        mvwprintw(input_move_window, seq_index, 1,
                  "%d: %s\t(%2d)", seq_index, mn[input_sequence[seq_index - 1].motion],
                  input_sequence[seq_index - 1].count + 1);
        wattron(input_move_window, COLOR_PAIR(board->bag[piece_count + board->bag_index] + 16));
    }
    wnoutrefresh(input_move_window);
    wnoutrefresh(board_win);
    debug_display(g_debug_verbosity);
    if (piece_count == 4) {
        mvprintw(BOARD_Y + bag_cursor,  BOARD_X + 22, "_____");
        mvprintw(BOARD_Y - 1, BOARD_X + 6, "!execute!");
        wnoutrefresh(board_win);
        wnoutrefresh(stdscr);
        nodelay(board_win, true);
        nodelay(stdscr, true);
        doupdate();
        seq_len = seq_index ;
        seq_index = 0;
        bag_cursor = 4;
        GAME_STATE = EXECUTE_MOVES;
        wattron(execute_move_window, COLOR_PAIR(board->bag[board->bag_index] + 16));
        return EXECUTE_MOVES;
    }
    doupdate();
    return INPUT_MOVES;
}

            /* qwfpexecute */
game_state_t execute_moves() {
    input = input_sequence[seq_index].motion;            
    wattron(execute_move_window, COLOR_PAIR(board->bag[board->bag_index] + 16));
    mvwprintw(execute_move_window, seq_index + 1, 1,
              "%d: %s\t(%2d)", seq_index + 1, mn[input_sequence[seq_index].motion],
              input_sequence[seq_index].count + 1);
    wnoutrefresh(execute_move_window);
    doupdate();
    napms(150);
    switch (input) {                    
        case ROTATE_CCW:                          
            mino_render('0');
            mino_resolve_motion(ROTATE_CCW);
            break;
        case ROTATE_CW:                           
            mino_render('0');
            mino_resolve_motion(ROTATE_CW);
            break;
        case ROTATE_180:                            
            mino_render('0');
            mino_resolve_motion(ROTATE_180);
            break;
        case MOVE_LEFT:                             
            while (input_sequence[seq_index].count >= 0) {
                mino_render('0');
                mino_resolve_motion(MOVE_LEFT);
                mino_render('1');
                wnoutrefresh(board_win);
                doupdate();
                napms(50);
                input_sequence[seq_index].count--;
            }
            break;
        case MOVE_RIGHT:
            while (input_sequence[seq_index].count >= 0) {
                mino_render('0');
                mino_resolve_motion(MOVE_RIGHT);
                mino_render('1');
                wnoutrefresh(board_win);
                doupdate();
                napms(50);
                input_sequence[seq_index].count--;
            }
            break;
        case HARD_DROP:
            mino_render('0');
            state_update = mino_resolve_motion(HARD_DROP);
            hold_available = true;
            stats.pc += 1;
            break;
        case SOFT_DROP:
            mino_render('0');
            if (mino_resolve_motion(SOFT_DROP) == 3) {
                wnoutrefresh(board_win);
            }
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
    mino_render('1');
    wnoutrefresh(board_win);

    input_sequence[seq_index].motion = 8;
    input_sequence[seq_index].count = 0;
    seq_index++;
    if (seq_index >= seq_len) {
        GAME_STATE = INPUT_MOVES;
        seq_index = 0; 
        seq_len = 0; 
        piece_count = 0;
        mvprintw(BOARD_Y - 1, BOARD_X + 6, " !think! ");
        mino_render('0');
        doupdate();
        return INPUT_MOVES;
    }
    return EXECUTE_MOVES;
}

void ui_make_message(WINDOW *window , char *msg, int y, int x) {
    mvwaddstr(window, y, x, msg);
}

