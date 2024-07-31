#!/bin/bash
#./cooking_runner.sh #number_of_iteration
echo "Running cooking_cv.c"
echo ""
RANDOM=$$
maxingred=29
maxpacket=10000
for (( i = 1; i <= $1; i++))
do
    packets=$(($RANDOM % $maxpacket + 1))
    ingred=$(($RANDOM % $maxingred + 2))
    echo -en "\rAttempt $i/$1 $packets $ingred       "
    X=$(./cooking_cv_runner.sh $packets $ingred)
    if [[ $X != 'OK. All test cases passed!' ]]
    then
        echo "PROBLEM"
        echo $packets, $ingred
        echo $X
        exit
    fi
done
echo ""
echo "Cooking CV Successful"

echo "Running cooking_zem.c"
echo ""
for (( i = 1; i <= $1; i++))
do
    packets=$(($RANDOM % $maxpacket + 1))
    ingred=$(($RANDOM % $maxingred + 2))
    echo -en "\rAttempt $i/$1 $packets $ingred      "
    X=$(./cooking_zem_runner.sh $packets $ingred)
    if [[ $X != 'OK. All test cases passed!' ]]
    then
        echo "PROBLEM"
        echo $packets, $ingred
        echo $X
        exit
    fi
done
echo ""
echo "Cooking Zem Successful"