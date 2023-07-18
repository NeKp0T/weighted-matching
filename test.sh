#!/bin/bash


for i in wtests/*
do
    echo "test $i"
    valgrind --tool=memcheck -q --leak-check=full ./W.elf < $i > /dev/null
    ./W.elf < $i | ./check $i
done

# for i in {1..5}
# do
#     echo "test $i"
#     valgrind --tool=memcheck -q --leak-check=full ./A.elf < tests/$i.in > /dev/null
#     ./A.elf < tests/$i.in | python checker.py $i
# done

