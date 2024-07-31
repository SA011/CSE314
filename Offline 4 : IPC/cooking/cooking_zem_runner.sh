#!/bin/bash
# echo "Running cooking_zem.c"
# echo ""
gcc cooking_zem.c zemaphore.c -o cooking_zem -lpthread
./cooking_zem $1 $2 > temp.txt
awk -f check.awk ITEM=$1 MAX=$2 temp.txt
rm temp.txt