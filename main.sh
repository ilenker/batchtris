#!/usr/bin/env bash

gcc -g -I./src src/main.c src/boardfunc.c src/minofunc.c -lncurses -Wextra -Wall -Wshadow -Wpointer-arith -o  batchtris

if [[ $? == 0 ]]; then
    ./batchtris
fi
