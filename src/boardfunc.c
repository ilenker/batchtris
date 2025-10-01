#include <ncurses.h>
#include <stdlib.h>
#include "boardfunc.h"
#include "minofunc.h"

#define __break {wclear(board->parent_window); board_render(board, board->parent_window); nodelay(board->parent_window, FALSE); 
#define point__ refresh(); doupdate(); wgetch(board->parent_window); nodelay(board->parent_window, TRUE);}

int bag_next(board_t *board) {
    board->bag_index = (board->bag_index + 1) % 14;
    if (board->bag_index == 0){
        bag_shuffle(&(board->bag)[7]);
    } 
    if (board->bag_index == 7){
        bag_shuffle(&(board->bag)[0]);
    } 
    return board->bag[board->bag_index];
}

void bag_shuffle(int *bag) {
    int l;
    int r = 1;
    int temp;
    for (int i = 0; i < 20; i++) {
        l = rand() % 7; 
        while (l == r) {
            r = rand() % 7; 
        }
        temp = bag[l];
        bag[l] = bag[r];
        bag[r] = temp;
    } 
}


int board_data_at_yx(board_t *board, int y, int x) {
    row_t *current = board->head; // I justify this with: we accessed the first
    y--;                         // index by getting the head, if i is now -1, just return
    while(y >= 0) {
        current = current->next;
        y--;
        if (current ==  NULL) {
            return -1;
        }
    }
    return current->data[x];
}

void board_render(board_t *board, WINDOW *window) {
    row_t *current_row;
    char col;
    row_iterator(NULL, 1);
    for (int y = 0; y < 20; y++) {
        current_row = row_iterator(board->head, 0); 
        if (current_row == NULL) {break;} // sll iteration
        if (y < board->render_limit) {continue;} // sll iteration
        for (int x = 0; x < 10; x++) {
            col = current_row->data[x];
            if (col != 9) {
                wattron(window, COLOR_PAIR(col));
                mvwaddch(window, y, x * 2, 'x'); 
                waddch(window, 'x'); 
            } else {
                wattron(window, COLOR_PAIR(8));
                mvwaddch(window, y, x * 2, 'x'); 
                waddch(window, '*'); 
            }
        }
    }
}

int row_process(board_t *board, int row, stackop_t operation) {
    static int stack[4];  
    static int ptr = 0;
    static int min = 69;
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
                row_clear(board, min, 4); 
                stack[0] = -2;
                stack[1] = -2;
                stack[2] = -2;
                stack[3] = -2;
                ptr = 0;
                min = 69;
                return ptr;
                break;
            }
            // Sort the stack x<y<z
            if (stack[0] < stack[1]) {
                stack[0] ^= stack[1];
                stack[1] ^= stack[0];
                stack[0] ^= stack[1];
            }
            if (stack[2] < stack[3]) {
                stack[2] ^= stack[3];
                stack[3] ^= stack[2];
                stack[2] ^= stack[3];
            }
            if (stack[0] < stack[2]) {
                stack[0] ^= stack[2];
                stack[2] ^= stack[0];
                stack[0] ^= stack[2];
            }
            if (stack[1] < stack[3]) {
                stack[1] ^= stack[3];
                stack[3] ^= stack[1];
                stack[1] ^= stack[3];
            }
            if (stack[1] < stack[2]) {
                stack[1] ^= stack[2];
                stack[2] ^= stack[1];
                stack[1] ^= stack[2];
            }
            if (ptr > 0) {
                while (ptr > 0) { 
                    ptr--; 
                    row_clear(board, stack[ptr], 1);          
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

void row_clear(board_t *board, int fullrow, int n) {
    row_t *pre_gap = row_at_index(board, fullrow - 1); 
    row_t *first_full_row = pre_gap->next;
    row_t *last_full_row;

    last_full_row = first_full_row;
    for (int i = 0; i < 10; i++) {
        first_full_row->data[i] = 9;     // reset row data
    }
    last_full_row->count = 0;
    while (n >= 2) {
        last_full_row = last_full_row->next;
        for (int i = 0; i < 10; i++) {
            last_full_row->data[i] = 9; 
        }
        last_full_row->count = 0;
        n--;
    }
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

row_t *row_iterator(row_t *head, int reset) {
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

void board_init(board_t *board, char depth, char width) {
    // TODO: refactor this function
    board->depth = depth;
    board->width = width;
    board->render_limit = 19;
    board->bag_index = 0;
    board->bag[0] = I; board->bag[7]  = I;
    board->bag[1] = O; board->bag[8]  = O;
    board->bag[2] = J; board->bag[9]  = J;
    board->bag[3] = L; board->bag[10] = L;
    board->bag[4] = S; board->bag[11] = S;
    board->bag[5] = Z; board->bag[12] = Z;
    board->bag[6] = T; board->bag[13] = T;
    bag_shuffle(board->bag);
    bag_shuffle(&board->bag[7]);

    row_t *current_row;
    row_iterator(NULL, 1);
    for (int i = 0; i < 20; i++) {
        current_row = row_iterator(board->head, 0); // sll iteration
        if (current_row == NULL) {break;}
        for (int j = 0; j < 10; j++) {
            if (current_row != NULL) {
                current_row->data[j] = 9;
            }
        }
    }
    for (int y = 0; y < board->depth; y++) {
        board->row_counts[y] = 0;
    }
}

void board_init_sll(board_t *board) {
    // TODO: refactor this function
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

    board->head = row_0;
}

void bag_render(board_t *board, WINDOW* queuewindow) {
    for (int i = 0; i < QUEUE_PREVIEW_LENGTH; i++) {
    } 
}
// Reverse a List
//:'t+1,.g/^/m 't

