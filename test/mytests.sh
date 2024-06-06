#!/bin/bash

# Ensure the script is executable
chmod +x test/mytests.sh

# Define auxiliar variable
TARGET="main.exe"

# Compile the program
make

# Write the title to the test/data.txt file
echo "RESULTS OF MY TESTS" > test/data.txt
echo "=================================" >> test/data.txt

# FIRST TEST
echo "TEST 1: Running with n=51, max_it=5000, tol=1e-6" >> test/data.txt

# RUN 1.1
echo "SERIAL SOLVER" >> test/data.txt
mpiexec -np 1 -x OMP_NUM_THREADS=1 ./$TARGET 51 5000 1e-6 1 >> test/data.txt

# RUN 1.2
echo "2 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 2 -x OMP_NUM_THREADS=1 ./$TARGET 51 5000 1e-6 2 >> test/data.txt

# RUN 1.3
echo "4 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=1 ./$TARGET 51 5000 1e-6 3 >> test/data.txt

# RUN 1.4
echo "4 CORES and 2 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$TARGET 51 5000 1e-6 4 >> test/data.txt
echo "=================================" >> test/data.txt

# SECOND TEST
echo "TEST 1: Running with n=101, max_it=40000, tol=1e-5" >> test/data.txt

# RUN 1.1
#echo "SERIAL SOLVER" >> test/data.txt
#mpiexec -np 1 -x OMP_NUM_THREADS=1 ./$TARGET 101 40000 1e-5 >> test/data.txt

# RUN 2.2
echo "2 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 2 -x OMP_NUM_THREADS=1 ./$TARGET 101 40000 1e-5 5 >> test/data.txt

# RUN 2.3
echo "4 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=1 ./$TARGET 101 40000 1e-5 6 >> test/data.txt

# RUN 2.4
echo "4 CORES and 2 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$TARGET 101 40000 1e-5 7 >> test/data.txt
echo "=================================" >> test/data.txt

echo "TESTS COMPLETED, see 'test' folder for results"