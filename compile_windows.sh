#!/usr/bin/env bash

x86_64-w64-mingw32-gcc -I./src src/*.c -lncurses -static -Wextra -Wall -Wshadow -Wpointer-arith -o batchtris.exe
