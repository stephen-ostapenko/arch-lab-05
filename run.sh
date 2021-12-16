#!/bin/bash

if [[ $# -ne 2 ]]
then
	echo "kek"
	exit 1
fi

./main $1 test_picture.pnm test_output.pnm >> $2
