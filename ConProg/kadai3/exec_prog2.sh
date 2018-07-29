#!/usr/bin/sh

./prog2 20 20 5 20 5 > log1
./prog2 20 5 20 5 20 > log2
./prog2 20 5 20 20 5 > log3
./prog2 20 20 5 5 20 > log4

./CorrectData log1 > result1
./CorrectData log2 > result2
./CorrectData log3 > result3
./CorrectData log4 > result4

