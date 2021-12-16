#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo "kek"
	exit 1
fi

if [[ ! -f "test_picture.pnm" ]]
then
	unzip test_picture.zip
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
g++ main.cpp -DSCHEDULE_TYPE=static -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (static, 1)
echo "=== (static, 1) =================================================================="
echo
g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (static, 8)
echo "=== (static, 8) =================================================================="
echo
g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic)
echo "=== (dynamic) ===================================================================="
echo
g++ main.cpp -DSCHEDULE_TYPE=dynamic -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic, 1)
echo "=== (dynamic, 1) ================================================================="
echo
g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1

#########################################################################################

# (dynamic, 8)
echo "=== (dynamic, 8) ================================================================="
echo
g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
sleep $T

invoke $1
