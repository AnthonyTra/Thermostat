#!/bin/bash
#
# Shows examples of running thermo_main on various sensor values


# echo '> make thermo_main' 
make thermo_main > /dev/null
echo

data="0 10 20 34 $((32*5)) $((32*50+7)) $((32*396+17)) $((32*492)) $((32*639)) $((32*892))"

for t in $data; do
    printf "==========CELSIUS FOR %d==========\n" "$t"
    echo '> ./thermo_main' $t "C"
    ./thermo_main $t "C"
    echo
    echo
    printf "==========FAHRENHEIT FOR %d==========\n" "$t"
    echo '> ./thermo_main' $t "F"
    ./thermo_main $t "F"
    echo
    echo
done
