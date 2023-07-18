#!/bin/bash

#!/bin/bash

echo 1 > graph
for i in {1..10000}
do
    n=$(( ( RANDOM % 10 )  + 3 ))
    python gen_random.py $n -w > rand_wgraph.in
    ./W.elf < rand_wgraph.in | ./check rand_wgraph.in > /dev/null
    x=$?
    if [[ $x != 0 ]] 
    then
        echo $x
        break
    fi
    if (( $i % 100 == 0 ))
    then
        echo "test $i"
    fi
done