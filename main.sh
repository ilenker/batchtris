#!/usr/bin/env bash

gcc -I./src src/main.c src/boardfunc.c src/minofunc.c src/sprites.c -lncurses -Wextra -Wall -Wshadow -Wpointer-arith -o batchtris

if [[ $? == 0 ]]; then
    ./batchtris
fi
