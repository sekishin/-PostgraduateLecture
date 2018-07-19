#! /usr/bin/zsh
rm result.csv
for i in $(seq 1000)
do
  echo $i
  python3 simulation.py >> result.csv
done
