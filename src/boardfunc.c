#include <ncurses.h>
#include "boardfunc.h"
#include "minofunc.h"

#define __break //{wclear(board->parent_window); render_board(board, board->parent_window); nodelay(board->parent_window, FALSE); 

#define point__ //refresh(); doupdate(); wgetch(board->parent_window); nodelay(board->parent_window, TRUE);}


int get_board_data_yx(board_t *board, int y, int x) {
    row_t *current = board->head; // I justify this with: we accessed the first
    y--;                         // index by getting the head, if i is now -1, just return
    while(y >= 0) {
        current = current->next;
        y--;
        if (current == NULL) {
            return -1;
        }
    }
    return current->data[x];
}

void render_board(board_t *board, WINDOW *window) {
    row_t *current_row;
    char col;
    yield_next_row(NULL, 1);
    for (int y = 0; y < 20; y++) {
        current_row = yield_next_row(board->head, 0); 
        if (current_row == NULL) {break;} // sll iteration
        for (int x = 0; x < 10; x++) {
            if (current_row != NULL) {
                col = current_row->data[x];
                if (col != 9) {
                    wattron(window, COLOR_PAIR(col));
                    mvwaddch(window, y, x * 2, 'x'); 
                    waddch(window, 'x'); 
                }
            }
        }
    }
}

int process_rows(board_t *board, int row, stackop_t operation) {
    static int stack[4];  
    static int ptr = 0;
    static int min = 69;
              //mvaddstr(14, 30, "...............................");
              //mvaddstr(15, 30, "...............................");
              //mvaddstr(16, 30, "...............................");
              //mvaddstr(17, 30, "...............................");
              //mvaddstr(18, 30, "...............................");
              //mvaddstr(19, 30, "...............................");
              //mvaddstr(20, 30, "...............................");
              //mvaddstr(21, 30, "...............................");
              //mvaddstr(22, 30, "...............................");

              //__break
              //char *op;
              //if (operation == PUSH) {op = "PUSH";}
              //if (operation == POP) {op = "POP";}
              //if (operation == PEEK) {op = "PEEK";}
              //if (operation == CLEAR) {op = "CLEAR";}
           //   mvprintw(14, 30, "process_rows(row = %d, op=%s)", row, op);
           //   mvprintw(15, 30, "ptr : %d", ptr);
           //   mvprintw(16, 30, "<proceed?>");
              //point__
              //mvprintw(16, 30, "_____________________________");
    switch (operation) {
        case POP:
            ptr--;
            return ptr;
            break;
        case PUSH:
            // For first push, just do it.           
            // For 2, if greater than top, push      
            //        else insert below               
            // For 3... you know what. I'm just going
            // to do head each one. Only if it's a 4-
            // clear we do the efficient way. T-spins
            // will be assumed contiguous, so we will
            // implement that if we get there.(^=_=^)
            stack[ptr] = row;
       //       __break
       //       mvprintw(17, 30, "set stack[ptr:%d] = %2d (row)", ptr, row);
       //       mvprintw(18, 30, "<proceed?>");
       //       point__
       //       mvprintw(18, 30, "_____________________________");
            ptr++;
            if (row < min) {
                min = row;
            }
            return ptr;
            break;
        case PEEK:
            return ptr;
            break;
        case CLEAR:
            if (ptr == 4) {
                clear_rows(board, min, 4); 
                ptr = 0;
                min = 69;
                return ptr;
                break;
            }
            if (ptr > 0) {
                while (ptr > 0) { 
                    ptr--; 
       //           __break
       //           mvprintw(19, 30, "___________loopin..._________");
       //           mvprintw(20, 30, "stack[ptr:%d]: [%2d]", ptr, stack[ptr]);
       //           mvprintw(21, 30, "clear_rows(%2d, 1)", stack[ptr]);
       //           mvprintw(22, 30, "          <proceed>          ");
       //           point__
                    clear_rows(board, stack[ptr], 1);
                    if (stack[ptr] > stack[ptr - 1]) {
                        stack[ptr - 1]++; 
                    }
                    /* when we clear rows one-by-one, the stack of row indices
                       remained the same throughout the process. So as each row 
                       cleared and the rows above dropped down, the row indices
                       become stale if they happened to point to a row above a
                       cleared row. To fix, we check if the next row index is
                       smaller than the one we just cleared. If so, increment
                       the next row index in the stack.
                      */
                    stack[ptr] = -2;
                }
                ptr = 0;
                min = 69;
                return ptr;
                break;
            } else {
                ptr = 0;
                min = 69;
                return ptr;
                break;
            }
            return ptr;
    }
}

void clear_rows(board_t *board, int fullrow, int count) {
    row_t *pre_gap = row_at_index(board, fullrow - 1); 
    row_t *first_full_row = pre_gap->next;
    row_t *last_full_row;

                mvprintw(0, 0, "clear_rows called on [%d]", fullrow);
    last_full_row = first_full_row;
    for (int i = 0; i < 10; i++) {
                __break
                mvprintw(1, 0, "first_full_row->data[%d] = %d", i, last_full_row->data[i]);
                point__
        first_full_row->data[i] = 9;     // reset row data
    }
    last_full_row->count = 0;
    while (count > 1) {
        last_full_row = last_full_row->next;
                mvprintw(0, 0, "clear_rows called on [%d]->next", fullrow);
        for (int i = 0; i < 10; i++) {
                __break
                mvprintw(1, 0, "last_full_row->data[%d] = %d", i, last_full_row->data[i]);
                point__
            last_full_row->data[i] = 9; 
        }
        last_full_row->count = 0;
        count--;
    }
                wclear(board->parent_window);
    pre_gap->next = last_full_row->next;
    last_full_row->next = board->head;
    board->head = first_full_row;
}

row_t *row_at_index(board_t *board, int i) {
    row_t *current = board->head; // I justify this with: we accessed the first
    i--;                         // index by getting the head, if i is now -1, just return
    while(i >= 0) {
        current = current->next;
        i--;
        if (current == NULL) {
            return NULL;
        }
    }
    return current;
}

row_t *yield_next_row(row_t *head, int reset) {
    static row_t *current = NULL;
    static row_t *start = NULL;
    if (reset == 1) {
        current = NULL;
        start = NULL;
        return NULL;
    }
    if (start == NULL) {
        start = head;
    }
    if (current == NULL) {
        current = head;
        return current;
    } else {
        current = current->next;
    }
    return current;
}

void init_board(board_t *board, char depth, char width) {
    board->depth = depth;
    board->width = width;
    board->render_limit = 0;
    row_t *current_row;
    yield_next_row(NULL, 1);
    for (int i = 0; i < 20; i++) {
        current_row = yield_next_row(board->head, 0); // sll iteration
        if (current_row == NULL) {break;}
        for (int j = 0; j < 10; j++) {
            if (current_row != NULL) {
                current_row->data[j] = 9;
            }
        }
    }
    for (int y = 0; y < board->depth; y++) {
        board->row_counts[y] = 0;
      //  for (int x = 0; x < board->width; x++) {
      //   board->NOGREP[board->depth - 1 - y][x] = 9;
      //  } 
    }
}

void init_board_sll(board_t *board, char depth, char width) {
    #define I_will_not_encourage_others_to_fly = calloc(1, sizeof(row_t));
    row_t *row_0  I_will_not_encourage_others_to_fly
    row_t *row_1  I_will_not_encourage_others_to_fly 
    row_t *row_2  I_will_not_encourage_others_to_fly
    row_t *row_3  I_will_not_encourage_others_to_fly
    row_t *row_4  I_will_not_encourage_others_to_fly
    row_t *row_5  I_will_not_encourage_others_to_fly
    row_t *row_6  I_will_not_encourage_others_to_fly
    row_t *row_7  I_will_not_encourage_others_to_fly
    row_t *row_8  I_will_not_encourage_others_to_fly
    row_t *row_9  I_will_not_encourage_others_to_fly
    row_t *row_10 I_will_not_encourage_others_to_fly
    row_t *row_11 I_will_not_encourage_others_to_fly
    row_t *row_12 I_will_not_encourage_others_to_fly
    row_t *row_13 I_will_not_encourage_others_to_fly
    row_t *row_14 I_will_not_encourage_others_to_fly
    row_t *row_15 I_will_not_encourage_others_to_fly
    row_t *row_16 I_will_not_encourage_others_to_fly
    row_t *row_17 I_will_not_encourage_others_to_fly
    row_t *row_18 I_will_not_encourage_others_to_fly
    row_t *row_19 I_will_not_encourage_others_to_fly

    row_0->next  = row_1;
    row_1->next  = row_2;
    row_2->next  = row_3;
    row_3->next  = row_4;
    row_4->next  = row_5;
    row_5->next  = row_6;
    row_6->next  = row_7;
    row_7->next  = row_8;
    row_8->next  = row_9;
    row_9->next  = row_10;
    row_10->next = row_11;
    row_11->next = row_12;
    row_12->next = row_13;
    row_13->next = row_14;
    row_14->next = row_15;
    row_15->next = row_16;
    row_16->next = row_17;
    row_17->next = row_18;
    row_18->next = row_19;
    row_19->next = NULL;

    for (int i; i < 10; i++) {
        row_0->data[i]  = '0';
        row_1->data[i]  = '1';
        row_2->data[i]  = '2';
        row_3->data[i]  = '3';
        row_4->data[i]  = '4';
        row_5->data[i]  = '5';
        row_6->data[i]  = '6';
        row_7->data[i]  = '7';
        row_8->data[i]  = '8';
        row_9->data[i]  = '9';
        row_10->data[i] = 'A';
        row_11->data[i] = 'B';
        row_12->data[i] = 'C';
        row_13->data[i] = 'D';
        row_14->data[i] = 'E';
        row_15->data[i] = 'F';
        row_16->data[i] = 'G';
        row_17->data[i] = 'H';
        row_18->data[i] = 'I';
        row_19->data[i] = 'J';
    }
    board->head = row_0;
}
// Reverse a List
//:'t+1,.g/^/m 't

