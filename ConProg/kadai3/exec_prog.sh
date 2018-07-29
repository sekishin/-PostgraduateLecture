#!/usr/bin/sh

echo "real time, cpu time" > result1.csv
for i in $(seq 100)
do
  echo $i
  ./prog1 100 $i >> result1.csv 
done
