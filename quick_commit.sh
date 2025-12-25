#!/bin/bash

#TODO: automation for branches.

current_branch=$(git branch --show-current)

if [ "$current_branch" != "main" ]; then
    echo "You're not on main branch! Script not updated to handle this yet!"
    exit 1
fi

if [ $# -ge 1 ]; then
    mess=$1
else 
    mess="Quick commit on $(date '+%Y-%m-%d %H:%M:%S')"
fi

git add .
git commit -m "$mess"
git push origin main

# verify
git status

# etersai 2025
