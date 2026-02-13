#!/bin/bash
#!/usr/bin/env bash <= more portable
cc -Wall -Wextra -ggdb -fsanitize=address str.c -o str
./str

