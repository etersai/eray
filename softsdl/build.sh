#!/bin/bash
prog_name=cool
gcc -g -Wall -Wextra -I./include -o $prog_name main.c -lSDL3 -lm
./$prog_name
