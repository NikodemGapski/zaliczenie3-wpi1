#!/bin/bash
make solve
for i in {0..7}
do
	./solve.e <./tests/$i.in >.out
	check=`diff -w ./tests/$i.out .out`
	if [ "$check" != "" ]; then
		echo "FAILED $i $check"
	else
		echo "OK $i"
	fi
done
make clean
rm -f .out