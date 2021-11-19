#!/bin/bash

make
compiled=$?
if [[ $compiled != 0 ]]; then
	echo "does not compile"
	exit 1
fi

echo "Program Compiles"

echo -n "Example 1: "
./a.out example1.txt -a -s -l -v <<< 6 > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo ":'("
	exit 1
else
	diff -w -B output.txt out1.txt &> /dev/null
	correct=$?
	if [[ $correct != 0 ]]; then
		echo ":'("
		exit 1
	else
		echo "Passed!"
	fi
fi


echo -n "Example 2: "
./a.out example2.txt -a -s -l -v > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo ":'("
else
	diff -w -B output.txt out2.txt &> /dev/null
	correct=$?
	if [[ $correct != 0 ]]; then
		echo ":'("
	else
		echo "Passed!"
	fi
fi

echo -n "Example Error: "
./a.out exampleError.txt -a -s -l -v > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo ":'("
else
	diff -w -B output.txt outError.txt &> /dev/null
	correct=$?
	if [[ $correct != 0 ]]; then
		echo ":'("
	else
		echo "Passed!"
	fi
fi


echo -n "Really Complex Tipping Test: "
./a.out exampleTip.txt -a -s -l -v <<< '1 10 51 17 2 10 51 17 0' > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo ":'("
else
	diff -w -B output.txt outTip.txt &> /dev/null
	correct=$?
	if [[ $correct != 0 ]]; then
		echo ":'("
	else
		echo "Passed!"
	fi
fi
