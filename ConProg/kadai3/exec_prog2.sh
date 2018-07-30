#!/usr/bin/sh

make time

./prog2 20 20 5 20 5 > log1
./prog2 20 5 20 5 20 > log2
./prog2 20 5 20 20 5 > log3
./prog2 20 20 5 5 20 > log4

./CorrectData log1 > result_prog2_1
./CorrectData log2 > result_prog2_2
./CorrectData log3 > result_prog2_3
./CorrectData log4 > result_prog2_4

