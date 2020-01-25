#!/bin/bash
for i in $( ls TestData ); do
    echo Starting test file: $i
    ./scheduler TestData/$i 1
    mv simulation_log TestOutput/Q1$i
    ./scheduler TestData/$i 10
    mv simulation_log TestOutput/Q10$i
    echo Test file $i complete
done
echo Testing with self
./scheduler scheduler 1
mv simulation_log TestOutput/Q1scheduler
./scheduler scheduler 10
mv simulation_log TestOutput/Q10scheduler
echo Tests complete
