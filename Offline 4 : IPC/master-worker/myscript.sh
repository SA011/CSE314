#!/bin/bash
#./myscript.sh number_of_iteration
RANDOM=$$
maxbuf=100
maxcons=100
maxprod=100
for (( i = 1; i <= $1; i++))
do
    echo -en "\rAttempt $i/$1"
    packets=$(($RANDOM + 1))
    buf=$(($RANDOM % $maxbuf + 1))
    cons=$(($RANDOM % $maxcons + 1))
    prod=$(($RANDOM % $maxprod + 1))
    X=$(./test-master-worker.sh $packets $buf $cons $prod)
    if [[ $X != "OK. All test cases passed!" ]]
    then
        echo "PROBLEM"
        echo $packets, $buf, $cons, $prod
        echo $X
        exit
    fi
done
echo