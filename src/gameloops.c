#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"
#include "gameloops.h"
#include "init_think_execute.h"
#include "menus.h"

// general init 
game_state_t GAME_STATE;
game_state_t PREV_GAME_STATE;
int state_update;
int input;
stats_t stats;
data_t *data_options;
data_t *data_controls;

bool hold_available;
shape_t input_phase_hold;
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

//↶ ↷ π
uint16_t cw[2] = {'C', 8631};
uint16_t ccw[2] = {'@', 8630};
uint16_t _180[2] = {'8', 960};
uint16_t left[2] = {'<', 8592 };
uint16_t right[2] = {'>', 8594};
uint16_t hd[2] = {'!', '!'};
uint16_t sd[2] = {'v', 8964};

void game_variables_init() {
    // General
    GAME_STATE = MENU;
    PREV_GAME_STATE = MENU;

    hold_available = true;
    input_phase_hold = NOPIECE;
    hold = NOPIECE;
    input = -1;
    stats.pc = 0;
    stats.kpp = 0.0f;
    stats.pps = 0.0f;

    // Classic Specific
    
    // !think! !execute! Specific
    seq_index = 0; 
    seq_len = 0; 
    piece_count = 0;
    // Other
    g_debug_verbosity = 0;
}


            /* qwfpclassic */
game_state_t classic_tetris() {

    input = getch(); 
    input = input_remap(input);
    switch (input) {
        case 'r':
            mino_render(0);
            mino_resolve_motion(ROTATE_CCW);
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case 's':                           
            mino_render(0);
            mino_resolve_motion(ROTATE_CW);
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case 'T':                            
        case 't':                            
            mino_render(0);
            mino_resolve_motion(ROTATE_180);
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case 'H':                             
        case 'h':                             
            mino_render(0);
            mino_resolve_motion(MOVE_LEFT);
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case ':':
            mino_render(0);
            mino_resolve_motion(MOVE_RIGHT);
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case ' ':
            mino_render(0);
            state_update = mino_resolve_motion(HARD_DROP);
            hold_available = true;
            mino_render(1);
            bag_q_render(1, 4);
            wnoutrefresh(board_win);
            break;
        case 'Z':
        case 'z':
            mino_render(0);
            if (mino_resolve_motion(SOFT_DROP) == 3) {
                wnoutrefresh(board_win);
            }
            mino_render(1);
            wnoutrefresh(board_win);
            break;
        case 'A':
        case 'a':
            if (hold_available) {
                mino_render(0);
                if (hold == NOPIECE) {
                    hold = mino->type;
                    mino_reset(bag_next());
                    bag_q_render(1, 4);
                } else {
                    hold ^= mino->type;
                    mino->type ^= hold;
                    hold ^= mino->type;
                    mino_reset(mino->type);
                }
                sprite_draw_yx(sprites, &sprite_data[hold], BOARD_Y + 2, BOARD_X - 9);
                hold_available = false;
                mino_render(1);
                wnoutrefresh(board_win);
            } 
            break;
        case 27:
            PREV_GAME_STATE = CLASSIC;
            return MENU;
            break;
        case '1':
            edit_board();
            break;
    }
    if (g_gravity_timer < 0) {    // TODO: Delta time    
        g_gravity_timer = 20;
        mino_render(0);
        mino_resolve_motion(GRAVITY);
        mino_render(1);
        wnoutrefresh(board_win);
    }
    g_gravity_timer -= data_options->items[0]->data.val;

    //if (first_seq->active && timer < 32) {
    //    if (first_seq->frame[timer]->type != event_null) {
    //        event_t *f = first_seq->frame[timer];
    //        f->event(stdscr, (char*)f->content, f->y, f->x); 
    //    }
    //    if (g_gravity_timer % 30 == 0) {
    //        timer++;
    //    }
    //}  
    if (state_update != SUCCESS_UPDATE) doupdate();
    state_update = -1;
    napms(17);
    return CLASSIC;
}

                /* qwfpinput */
game_state_t input_moves() {
    static int char_set;
    static int bag_cursor = 4;
    static int x_limit = 0;
    static int cury, curx;
    char_set = data_options->items[2]->data.state;
    refresh();
    input = getch();            
    input = input_remap(input);
    switch (input) {                    
        case 'R':                          
        case 'r':                          
            // TODO: figure out how to handle different fonts
            //       with varying unicode symbol widths
            wprintw(input_move_window, "%lc", ccw[char_set]);
            input_sequence[seq_index].motion = ROTATE_CCW;
            seq_index++;
            break;
        case 'S':                           
        case 's':                           
            wprintw(input_move_window, "%lc", cw[char_set]);
            input_sequence[seq_index].motion = ROTATE_CW;
            seq_index++;
            break;
        case 'T':                            
        case 't':                            
            wprintw(input_move_window, "%lc", _180[char_set]);
            input_sequence[seq_index].motion = ROTATE_180;
            seq_index++;
            break;

        case 'H':                             
        case 'h':                             
            if (x_limit < -4) {   // TODO: Make sure this is kosher
                break;
            }
            x_limit--;
            wprintw(input_move_window, "%lc", left[char_set]);
            input_sequence[seq_index].motion = MOVE_LEFT;
            if (input_sequence[seq_index - 1].motion == MOVE_LEFT) {
                input_sequence[seq_index - 1].count++;
                break;
            } 
            seq_index++;
            break;
        case ':':
            if (x_limit > 4) {
                break;
            }
            x_limit++;
            wprintw(input_move_window, "%lc", right[char_set]);
            input_sequence[seq_index].motion = MOVE_RIGHT;
            if (input_sequence[seq_index - 1].motion == MOVE_RIGHT) {
                input_sequence[seq_index - 1].count++;
                break;
            } 
            seq_index++;
            break;
        case 'Z':
        case 'z':
            wprintw(input_move_window, "%lc", sd[char_set]);
            input_sequence[seq_index].motion = SOFT_DROP;
            seq_index++;
            break;

        case 'A':
        case 'a':
            if (hold_available) {
                getyx(input_move_window, cury, curx);
                if (curx > 0) {
                    wprintw(input_move_window, "\n");
                }
                wprintw(input_move_window, "H-");
                sprite_draw_yx(sprites,   // Replace current piece on bag display with held piece
                               &sprite_data[input_phase_hold],
                               BOARD_Y + 1 + (bag_cursor),
                               BOARD_X + 21);
                if (input_phase_hold == NOPIECE) {
                    input_phase_hold = board->bag[(board->bag_index + piece_count) % 14];
                    piece_count++;
                    wattron(input_move_window, COLOR_PAIR(16 + board->bag[(piece_count + 
                                                          board->bag_index) % 14 ]));
                    bag_cursor_render(&bag_cursor);
                } else {
                    wattron(input_move_window, COLOR_PAIR(input_phase_hold + 16));
                    input_phase_hold = board->bag[(board->bag_index + piece_count) % 14];
                }
                hold_available = false;
                sprite_draw_yx(sprites, &sprite_data[input_phase_hold], BOARD_Y + 2, BOARD_X - 9);
               
                wprintw(input_move_window, ">SWAP\n");
                input_sequence[seq_index].motion = HOLD;
                seq_index++;
                x_limit = 0;
                wnoutrefresh(board_win);

            } else {
                // flash hold mino
            }
            break;
        case ' ':
            getyx(input_move_window, cury, curx);
            wprintw(input_move_window, "!");
            if (curx != 8) {
                wprintw(input_move_window, "\n");
            }
            piece_count++;
            input_sequence[seq_index].motion = HARD_DROP;
            hold_available = true;
            seq_index++;
            x_limit = 0;

            bag_cursor_render(&bag_cursor);
            wattron(input_move_window,
                    COLOR_PAIR(board->bag[(piece_count + board->bag_index) % 14] + 16));
            wnoutrefresh(input_move_window);
            break; 
        case 27: 
            PREV_GAME_STATE = THINK;
            return MENU;
            break;
        case KEY_RESIZE:
            display_reset(); 
            break;
    }

    if (piece_count == 4) {
        // UI Cleanup
        attron(COLOR_PAIR(9));
        mvprintw(BOARD_Y + bag_cursor    ,  BOARD_X + 20, "├────────╮");
        mvprintw(BOARD_Y + bag_cursor + 3,  BOARD_X + 20, "├────────╯");
        attron(COLOR_PAIR(8));
        mvprintw(BOARD_Y + bag_cursor - 3,  BOARD_X + 20, "│");
        mvprintw(BOARD_Y + bag_cursor - 6,  BOARD_X + 20, "│");
        mvprintw(BOARD_Y + bag_cursor    ,  BOARD_X + 20, "│");
        mvprintw(BOARD_Y + bag_cursor + 3,  BOARD_X + 20, "│");
        mvprintw(BOARD_Y - 2, BOARD_X + 1, "!execute!");
        wattron(execute_move_window, COLOR_PAIR(board->bag[board->bag_index] + 16));
        wnoutrefresh(board_win);
        wnoutrefresh(stdscr);
        nodelay(board_win, true);
        nodelay(stdscr, true);
        doupdate();

        seq_len = seq_index;
        seq_index = 0;
        bag_cursor = 4;
        return EXECUTE;
    }
    
    wnoutrefresh(input_move_window);
    wnoutrefresh(board_win);

    doupdate();
    return THINK;
}

            /* qwfpexecute */
game_state_t execute_moves() {
    static int char_set;
    char_set = data_options->items[2]->data.state;
    if (seq_index == 0) {
        werase(execute_move_window);
    }
    input = input_sequence[seq_index].motion;            
    switch (input) {                    
        case ROTATE_CCW:                          
            wprintw(execute_move_window, "%lc", ccw[char_set]);
            mino_render(0);
            mino_resolve_motion(ROTATE_CCW);
            napms(data_options->items[1]->data.val);
            break;
        case ROTATE_CW:                           
            wprintw(execute_move_window, "%lc", cw[char_set]);
            mino_render(0);
            mino_resolve_motion(ROTATE_CW);
            napms(data_options->items[1]->data.val);
            break;
        case ROTATE_180:                            
            wprintw(execute_move_window, "%lc", _180[char_set]);
            mino_render(0);
            mino_resolve_motion(ROTATE_180);
            napms(data_options->items[1]->data.val);
            break;
        case MOVE_LEFT:                             
            while (input_sequence[seq_index].count >= 0) {
                wprintw(execute_move_window, "%lc", left[char_set]);
                mino_render(0);
                mino_resolve_motion(MOVE_LEFT);
                mino_render(1);
                wnoutrefresh(execute_move_window);
                wnoutrefresh(board_win);
                doupdate();
                napms(data_options->items[1]->data.val);
                input_sequence[seq_index].count--;
            }
            break;
        case MOVE_RIGHT:
            while (input_sequence[seq_index].count >= 0) {
                wprintw(execute_move_window, "%lc", right[char_set]);
                mino_render(0);
                mino_resolve_motion(MOVE_RIGHT);
                mino_render(1);
                wnoutrefresh(board_win);
                wnoutrefresh(execute_move_window);
                doupdate();
                napms(data_options->items[1]->data.val);
                input_sequence[seq_index].count--;
            }
            break;
        case HARD_DROP:
            mino_render(0);           
            wprintw(execute_move_window, "!\n");
            state_update = mino_resolve_motion(HARD_DROP); // increments bag_index
            hold_available = true;
            wattron(execute_move_window, COLOR_PAIR(board->bag[board->bag_index] + 16));
            napms(data_options->items[1]->data.val * 2);
            break;
        case SOFT_DROP:
            wprintw(execute_move_window, "%lc", sd[char_set]);
            mino_render(0);
            mino_resolve_motion(SOFT_DROP);
            napms(data_options->items[1]->data.val);
            break;
        case HOLD:
            if (hold_available) {
                int cury, curx;
                getyx(execute_move_window, cury, curx);
                if (curx > 0) {
                    wprintw(execute_move_window, "\n");
                }
                mino_render(0);
                if (hold == NOPIECE) {
                    hold = mino->type;
                    mino_reset(bag_next());
                    bag_q_render(0, 4);
                } else {
                    hold ^= mino->type;
                    mino->type ^= hold;
                    hold ^= mino->type;
                    mino_reset(mino->type);
                }
                wprintw(execute_move_window, "\n");
                wprintw(execute_move_window, "H-");
                wattron(execute_move_window, COLOR_PAIR(mino->type + 16));
                wprintw(execute_move_window, ">SWAP\n");
                hold_available = false;
                mino_render(1);
                sprite_draw_yx(sprites, &sprite_data[hold], BOARD_Y + 2, BOARD_X - 9);
                wnoutrefresh(board_win);
                napms(data_options->items[1]->data.val);
            } 
            break;
    }
    mino_render(1);
    wnoutrefresh(board_win);
    wnoutrefresh(execute_move_window);
    doupdate();

        // Reset Motion and increment 
    input_sequence[seq_index].motion = NO_MOTION;
    input_sequence[seq_index].count = 0;
    seq_index++;


    if (seq_index >= seq_len) {
        seq_index = 0; 
        seq_len = 0; 
        piece_count = 0;
        mino_render(0);
        mvprintw(BOARD_Y - 2, BOARD_X + 1, " !think! ");
        doupdate();
        
        return THINK;
    }
    return EXECUTE;
}

void ui_make_message(WINDOW *window , char *msg, int y, int x) {
    mvwaddstr(window, y, x, msg);
    // TODO
}

void edit_board() {
    #define chtype_at_cursor mvwinch(board_win, xlate_y, xlate_x)
    #define empty '9'

              // thank you to sylt: https://gist.github.com/sylt/93d3f7b77e7f3a881603
              // for the guide on how to handle mouse input
    
    // Enables keypad mode. This makes (at least for me) mouse events getting
    // reported as KEY_MOUSE, instead as of random letters.
    keypad(stdscr, TRUE);
    nodelay(stdscr, 0);
    nodelay(board_win, 0);

    wmove(board_win, 0, 0);
    for (int i = 1; i < 8; i++) {
        wattron(board_win, COLOR_PAIR(i));
        waddch(board_win, '0' + i);
        waddch(board_win, '0' + i);
    }

    mouseinterval(0);

    // Don't mask any mouse events
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    printf("\033[?1003h\n"); // Makes the terminal report mouse movement events

    int picker_data = 9;
    int xlate_y, xlate_x;
    int c = -1;
    int cell_data;
    bool mb3_pressed;
    bool mb1_pressed;
    row_t *row_ptr;

    for (;;) { 
        c = getch();
        if (c == '\n')
            break;
        MEVENT event;
            /* Mouse event latching*/
        if (event.bstate & (BUTTON3_PRESSED)) {
            mb3_pressed = 1;
        }
        if (event.bstate & (BUTTON3_RELEASED)) {
            mb3_pressed = 0;
        }
        if (event.bstate & (BUTTON1_PRESSED)) {
            mb1_pressed = 1;
        }
        if (event.bstate & (BUTTON1_RELEASED)) {
            mb1_pressed = 0;
        }
        if (getmouse(&event) == OK) {
            xlate_y = (event.y - BOARD_Y);
            xlate_x = (event.x - BOARD_X) - (event.x - BOARD_X) % 2;

            if (xlate_y == 0) {
                if (mb3_pressed) {   // Pick char at cursor, allow picking of top row (0)
                    picker_data = (A_CHARTEXT & mvwinch(board_win, xlate_y, xlate_x)) - '0';
                }
                continue;
            }

            if ((xlate_y >= board->depth     || xlate_y < 0) ||
                (xlate_x >= board->width * 2 || xlate_x < 0)   ) {
                continue;
            }

            if (mb3_pressed) {   // Pick char at cursor, allow picking of top row (0)
                picker_data = board_data_at_yx(xlate_y, (int)(xlate_x / 2));
            }


            if (mb1_pressed) {                                               // Add char at cursor
                row_ptr = row_at_index(xlate_y);
                cell_data = row_ptr->data[xlate_x / 2];
                row_ptr->data[xlate_x / 2] = picker_data;

                if (cell_data == 9) {
                    if (picker_data != 9) {row_ptr->count++;}
                } else
                if (picker_data == 9) {row_ptr->count--;}

                mvwaddch(board_win, xlate_y, xlate_x    , (picker_data + '0') | COLOR_PAIR(picker_data));
                mvwaddch(board_win, xlate_y, xlate_x + 1, (picker_data + '0') | COLOR_PAIR(picker_data));
            }
        }
        wnoutrefresh(board_win);
        doupdate();
    }
    wmove(board_win, 0, 0);
    wclrtoeol(board_win);
    nodelay(stdscr, 1);
    nodelay(board_win, 1);
    printf("\033[?1003l\n"); // Disable mouse movement events, as l = low
}


void bag_q_render(int from_idx, int queue_len) {
    for (int i = 0; i < queue_len; i++) {
        sprite_draw_yx(sprites,
                       &sprite_data[board->bag[(board->bag_index + i + from_idx) % 14]],
                       BOARD_Y + 5 + (i * 3),
                       BOARD_X + 21);
    }
}

void bag_cursor_render(int *bag_cursor) {
    attron(COLOR_PAIR(12));
    mvprintw(BOARD_Y + *bag_cursor    ,  BOARD_X + 20, "├────────╮");
    mvprintw(BOARD_Y + *bag_cursor + 3,  BOARD_X + 20, "├────────╯");
    attron(COLOR_PAIR(8));
    mvprintw(BOARD_Y + *bag_cursor    ,  BOARD_X + 20, "│");
    mvprintw(BOARD_Y + *bag_cursor + 3,  BOARD_X + 20, "│");
    *bag_cursor += 3;
    mvprintw(BOARD_Y + *bag_cursor    ,  BOARD_X + 20, "├────────╮");
    mvprintw(BOARD_Y + *bag_cursor + 3,  BOARD_X + 20, "├────────╯");
}

void all_delay_set(bool state) {
    nodelay(stdscr, state);
    nodelay(board_win, state);
}

int input_remap(int unmapped_input) {
    for (int i = 0; i < 8; i++) {
        switch (unmapped_input) {
            case KEY_DOWN:
                if (data_controls->items[i]->data.control == 2) {
                    return data_controls->items[i]->data_default.control;
                }
                break;
            case KEY_UP:
                if (data_controls->items[i]->data.control == 3) {
                    return data_controls->items[i]->data_default.control;
                }
                break;
            case KEY_LEFT:
                if (data_controls->items[i]->data.control == 4) {
                    return data_controls->items[i]->data_default.control;
                }
                break;
            case KEY_RIGHT:
                if (data_controls->items[i]->data.control == 5) {
                    return data_controls->items[i]->data_default.control;
                }
                break;
            default:
                if (unmapped_input == data_controls->items[i]->data.control) {
                    return data_controls->items[i]->data_default.control;
                }
                break;
        }
    }
    return unmapped_input;
}
