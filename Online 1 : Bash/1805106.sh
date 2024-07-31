#!/bin/bash
read -r q
echo $q
while ((q > 0)); do
  if ((q == 0))
  then 
    break
  fi
  read -r line
  temp=($line)
  com="${temp[0]}"

  fd=()
  indx=()


  if [[ $com = 'open' ]]; then
    name="${temp[1]}"
    touch $name
    cur=0
    x=0
    for i in "${!fd[@]}" 
    do
        echo $i "ss"
        if (( i != cur)); then
            fd[$i]=$name
            x=1
            echo $i
        fi
        cur=$((cur+1))
    done 
    if (( x == 0)); then
        echo $cur
        fd[$cur]=$name
    fi

    # for(( cur = 0; ; cur++ )) 
    # do
    #     echo ${fd[0]}
    #     echo $cur
    #     if [[ "${fd[$cur]}" = '' ]]; then
    #         echo $cur
    #         fd[$cur]=$name
    #         echo 
    #         break
    #     fi

    #     echo ${fd[0]}
    # done




  elif [[ $com = 'close' ]]; then
  
    name="${temp[1]}"
    for i in "${!fd[@]}" 
    do
        if [[ fd[$i] = $name]]; then
            fd[$i]=''
            x=1
            echo $i
            break;
        fi
    done 

  elif [[ $com = 'append' ]]; then
    
    name="${temp[1]}"
    for $i in "${!temp[@]}"
    do
        if ((i > 0)) then
            $fd[$name] << $temp[$i]
        fi
    done

  fi

  ((--q))
done