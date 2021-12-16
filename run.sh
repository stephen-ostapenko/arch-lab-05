#!/bin/bash

if [[ $# -ne 2 ]]
then
	echo "kek"
	exit 1
fi

./main $1 inputs/i16.pnm outputs/o16.pnm >> $2