#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo "kek"
	exit 1
fi

T=2

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
		echo -e "\n===\n"
	done

	echo >> $1
	echo -e "\n============\n"
}

#########################################################################################

# (static)
echo "=== (static) ====================================================================="
echo
g++ main.cpp -DST -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (static, 1)
echo "=== (static, 1) =================================================================="
echo
g++ main.cpp -DST -DCHSZ=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (static, 8)
echo "=== (static, 8) =================================================================="
echo
g++ main.cpp -DST -DCHSZ=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic)
echo "=== (dynamic) ===================================================================="
echo
g++ main.cpp -DDYN -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic, 1)
echo "=== (dynamic, 1) ================================================================="
echo
g++ main.cpp -DDYN -DCHSZ=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic, 8)
echo "=== (dynamic, 8) ================================================================="
echo
g++ main.cpp -DDYN -DCHSZ=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1
