#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "minofunc.h"
#include "boardfunc.h"
#include "sprites.h"
#include "gameui.h"

void ui_make_message(WINDOW *window , char *msg, int y, int x) {
    mvwaddstr(window, y, x, msg);
}
