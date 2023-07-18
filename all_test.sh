#!/bin/bash

echo 1 > graph
for i in {1..1000000}
do
    ./next_graph
    ./W.elf < graph | ./check graph > /dev/null
    x=$?
    if [[ $x != 0 ]] 
    then
        echo $x
        break
    fi
    if (( $i % 500 == 0 ))
    then
        echo "test $i"
    fi
done