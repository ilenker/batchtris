
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


// LERP Stuff
[

float lerp(float a, float b, float f) {
    return (a * (1.0 - f)) + (b * f);
}

    float start, end, f, df, ddf;
    start = 10.0f;
    end = 50.0f;
    f = 0.01f;
    df = 0.0f;
    ddf = 0.0001f;

    char trails[4] = {',', '-', '.', '\''};
    bool foo = true;
    bool bar = true;
    while (true) {
        df += ddf;
        f += df; 
        if (f < 0.5f) {
            mvaddstr(10, 3 + lerp(start, end, f), "<( ^o^)/");
            ddf = 0.0016f;
        }
        if (f > 0.5f) {
            mvaddstr(10, lerp(start, end, f) - 3, "\\(^o^ )>");
            ddf = -0.0024f;
        }
        if (fabs(df) > 0.013f) {
            mvaddch(10, lerp(start, end, f*(1 - 10*df)), trails[rand() % 4]);
        }
        if (fabs(df) > 0.016f) {
            mvaddch(10, lerp(start, end, f*(1 - 9*df)), trails[rand() % 4]);
        }
        if (fabs(df) > 0.019f) {
            mvaddch(10, lerp(start, end, f*(1 - 8*df)), 'o');
        }
        mvprintw(12, 20, "%f", f);
        mvprintw(13, 20, "%f", df);
        wnoutrefresh(stdscr);
        doupdate();
        napms(40);
        //if (foo && bar) 
        erase();
        //foo ^= bar;
        //bar = !bar;
    }

    _start_: ;
    df = 1.08f;
    while (lerp(start, end, f) <= end) {
        erase();
        mvaddstr(lerp(start, end, f), 10, "x");
        f *= df; 
        df += df/200; 
        wnoutrefresh(stdscr);
        doupdate();
        napms(17);
    }
    df = 0.92f;
    while (lerp(start, end, f) >= start + 1.5f) {
        erase();
        mvaddstr(lerp(start, end, f), 10, "x");
        f *= df; 
        df -= df/200;
        wnoutrefresh(stdscr);
        doupdate();
        napms(17);
    }
    goto _start_ ;
]
