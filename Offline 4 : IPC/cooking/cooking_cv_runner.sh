#!/bin/bash
# echo "Running cooking_cv.c"
# echo ""
gcc cooking_cv.c -o cooking_cv -lpthread
./cooking_cv $1 $2 > temp.txt
awk -f check.awk ITEM=$1 MAX=$2 temp.txt
rm temp.txt