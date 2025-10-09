#!/usr/bin/env bash

gcc -I./src src/*.c -lncursesw -Wextra -Wall -Wshadow -Wpointer-arith -o batchtris -pedantic -Wunreachable-code -Wstrict-overflow -Wcast-align

if [[ $? == 0 && $1 != "noex" ]]; then
    ./batchtris
fi
