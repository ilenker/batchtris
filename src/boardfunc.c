#include <ncurses.h>
#include "boardfunc.h"


int get_board_data_yx(board_t *board, int y, int x) {
    row_t *row;
    row = row_at_index(board, y);
    return row->data[x];
}

void clear_rows(board_t *board, int fullrow, int count) {
    // Single full row move to tail
    row_t *pre_gap = row_at_index(board, fullrow - 1); 
    row_t *full_row = pre_gap->next;
    row_t *post_gap;
    post_gap = full_row;
    while (count > 1) {
        post_gap = post_gap->next;
        count--;
    }
    pre_gap->next = post_gap->next;
    post_gap->next = board->head;
    board->head = full_row;
}

row_t *row_at_index(board_t *board, int i) {
    row_t *current = board->head;
    i--;
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
        for (int x = 0; x < board->width; x++) {
    //     board->NOGREP[board->depth - 1 - y][x] = 9;
        } 
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

    switch (operation) {
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
            min = row<min?row:min;  /* roamin */
            return ptr;
        case POP:
            return ptr;
        case CLEAR:
            if (ptr == 4) {
                clear_rows(board, min, 4); 
                return ptr;
            }
            if (ptr != 0) {
                while (ptr >= 0) { 
                    board->row_counts[stack[ptr]] = 0;
                    stack[ptr] = -1;
                    ptr--; 
                }
                ptr = 0;
                min = 69;
            }
            return ptr;
            break;
    }
}
