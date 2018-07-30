#!/usr/bin/sh

echo "real time, cpu time" > result_prog1.csv
for i in $(seq 100)
do
  echo $i
  ./prog1 256 $i >> result_prog1.csv 
done
