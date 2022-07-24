#!/bin/bash

# echo '> gcc clock_update.c clock_sim.c clock_main.c'
# gcc clock_update.c clock_sim.c clock_main.c

echo '> make clock_main'
make clock_main
echo

for t in 0 101 4170 43199 43200 47089 67089 86399; do
    echo '> ./clock_main' $t
    ./clock_main $t
    echo
    echo
done

