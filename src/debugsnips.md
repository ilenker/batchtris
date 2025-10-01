
// display queue
[
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
]
