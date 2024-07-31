#!/bin/bash

curDir="./Submissions/180512"
student="180512"
ac="./AcceptedOutput.txt"

max_score=100
max_id=5

exist(){
	if [[ -d $curDir$1 ]]
	then
		dir=$curDir$1/$student$1.sh
		if [[ -f $dir ]]
		then
			echo 1
		else
			echo 0
		fi
	else
		echo 0
	fi
}

match(){
	if (( $(exist $1) == 0 || $(exist $2) == 0 ))
	then 
		echo 0
		return
	fi
	dir1=$curDir$1/$student$1.sh
	dir2=$curDir$2/$student$2.sh
	
	if (( $(diff -w $dir1 $dir2 | wc -l) == 0 ))
	then
		echo 1
		return
	fi
	echo 0
	
}

evaluate(){
	if (( $(exist $1) == 0 ))
	then 
		echo $max_score
		return
	fi
	
	dir=$curDir$1/$student$1.sh
	
	chmod +x $dir
	ret=$($dir | diff -w $ac - | grep -c '>\|<' )
	ret=$(($ret * 5))
	echo $ret	
}



if (( $# >= 1 ))
then
	max_score=$1
fi

if (( $# >= 2 ))
then
	max_id=$2
fi

#echo $max_score
#echo $max_id

> output.csv


if [[ -d "./Submissions" ]]
then
	for ((i = 1; i <= $max_id ; i = i + 1))
	do
		x=$(evaluate $i)
		#echo $x
		if (( $x > $max_score ))
		then 
			x=0
		else
			x=$(($max_score - $x))
		fi
		
		for ((j = 1; j <= $max_id ; j = j + 1 ))
		do
			if (($i != $j)) && (( $(match $i $j) == 1 ))  
			then
				x=$((-x))
				break
			fi
		done
		
		echo "$student$i,$x" >> output.csv
		
	done
	
else
	for (( i = 1; i <= $max_id ; i = i + 1 ))
	do
		echo "$student$i, 0" >> output.csv 
	done
fi







