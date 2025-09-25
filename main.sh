#!/usr/bin/env bash

gcc -I./src src/main.c src/boardfunc.c src/minofunc.c -lncurses -o  batchtris

if [[ $? == 0 ]]; then
    ./batchtris
fi
