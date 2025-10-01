#!/usr/bin/env bash

gcc -I./src src/*.c -lncurses -Wextra -Wall -Wshadow -Wpointer-arith -o batchtris

if [[ $? == 0 ]]; then
    ./batchtris
fi
