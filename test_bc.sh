#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo "kek"
	exit 1
fi

T=2

rm $1

for (( it = 0; it < 3; it++ ))
do
	# (static)
	echo "(static)"
	g++ main.cpp -DSCHEDULE_TYPE=static -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 1)
	echo "(static, 1)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 2)
	echo "(static, 2)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=2 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 4)
	echo "(static, 4)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=4 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 8)
	echo "(static, 8)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 10)
	echo "(static, 10)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=10 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (static, 16)
	echo "(static, 16)"
	g++ main.cpp -DSCHEDULE_TYPE=static -DSCHEDULE_CHUNK_SIZE=16 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	#########################################################################################

	# (dynamic)
	echo "(dynamic)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 1)
	echo "(dynamic, 1)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=1 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 2)
	echo "(dynamic, 2)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=2 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 4)
	echo "(dynamic, 4)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=4 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 8)
	echo "(dynamic, 8)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=8 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 10)
	echo "(dynamic, 10)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=10 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 16)
	echo "(dynamic, 16)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=16 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	# (dynamic, 32)
	echo "(dynamic, 32)"
	g++ main.cpp -DSCHEDULE_TYPE=dynamic -DSCHEDULE_CHUNK_SIZE=32 -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 0 $1
	sleep $T

	#########################################################################################

	# without omp
	echo "without omp"
	g++ main.cpp -DSCHEDULE_TYPE=static -Wall -Wextra -pedantic -g -O2 -o main
	sleep $T
	./run.sh 1 $1
	sleep $T

	# omp 1 thread
	echo "omp 1 thread"
	g++ main.cpp -DSCHEDULE_TYPE=static -Wall -Wextra -pedantic -fopenmp -g -O2 -o main
	sleep $T
	./run.sh 1 $1
	sleep $T

	#########################################################################################

	echo >> $1
	echo -e "\n============\n"
done
