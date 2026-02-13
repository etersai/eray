#!/bin/bash
#!/usr/bin/env bash <= more portable
cc -Wall -Wextra -fsanitize=address str.c -o str
./str

