#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo "kek"
	exit 1
fi

T=5

rm $1

function invoke {
	for (( it = 0; it < 3; it++ ))
	do
		for (( i = 1; i <= 12; i++ ))
		do
			./run.sh $i $1
			echo
			sleep $T
		done

		echo >> $1
		echo "\n===\n"
	done

	echo "\n============\n"
}

#########################################################################################

# (static)
echo "=== (static) ====================================================================="
g++ main.cpp -DST -DCH_0 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1

#########################################################################################

# (static, 1)
echo "=== (static, 1) =================================================================="
g++ main.cpp -DST -DCH_1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1

#########################################################################################

# (static, 8)
echo "=== (static, 8) =================================================================="
g++ main.cpp -DST -DCH_8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1

#########################################################################################

# (dynamic)
echo "=== (dynamic) ===================================================================="
g++ main.cpp -DDYN -DCH_0 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1

#########################################################################################

# (dynamic, 1)
echo "=== (dynamic, 1) ================================================================="
g++ main.cpp -DDYN -DCH_1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1

#########################################################################################

# (dynamic, 8)
echo "=== (dynamic, 8) ================================================================="
g++ main.cpp -DDYN -DCH_8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

echo >> $1
