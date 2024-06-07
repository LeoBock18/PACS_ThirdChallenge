#!/bin/bash
# Bash script file to reproduce the tests I made on my computer. My results can be found in test/mydata.txt

# Ensure the script is executable
chmod +x test/mytests.sh

# Define auxiliar variable
TARGET="main.exe"

# Delete previous tests
make distclean

# Compile the program
make

# Write the title to the test/data.txt file
echo "RESULTS OF MY TESTS" > test/data.txt
echo "=================================" >> test/data.txt

# FIRST TEST
echo "TEST 1: Running with n=51, max_it=10000, tol=5e-7" >> test/data.txt

# RUN 1.1
echo "SERIAL SOLVER" >> test/data.txt
mpiexec -np 1 -x OMP_NUM_THREADS=1 ./$TARGET 51 10000 5e-7 1 >> test/data.txt

# RUN 1.2
echo "2 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 2 -x OMP_NUM_THREADS=1 ./$TARGET 51 10000 5e-7 2 >> test/data.txt

# RUN 1.3
echo "4 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=1 ./$TARGET 51 10000 5e-7 3 >> test/data.txt

# RUN 1.4
echo "4 CORES and 2 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$TARGET 51 10000 5e-7 4 >> test/data.txt
echo "=================================" >> test/data.txt

# SECOND TEST
echo "TEST 2: Running with n=101, max_it=1e6, tol=5e-6" >> test/data.txt

# RUN 2.1
echo "SERIAL SOLVER" >> test/data.txt
mpiexec -np 1 -x OMP_NUM_THREADS=1 ./$TARGET 101 1000000 5e-6 5 >> test/data.txt

# RUN 2.2
echo "2 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 2 -x OMP_NUM_THREADS=1 ./$TARGET 101 1000000 5e-6 6 >> test/data.txt

# RUN 2.3
echo "4 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=1 ./$TARGET 101 1000000 5e-6 7 >> test/data.txt

# RUN 2.4
echo "4 CORES and 2 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$TARGET 101 1000000 5e-6 8 >> test/data.txt
echo "=================================" >> test/data.txt

# THIRD TEST
echo "TEST 3: Running with n=401, max_it=10000000, tol=1e-4 (No Serial solver)" >> test/data.txt

# RUN 3.1
#echo "SERIAL SOLVER" >> test/data.txt
#mpiexec -np 1 -x OMP_NUM_THREADS=1 ./$TARGET 401 10000000 1e-4 >> test/data.txt

# RUN 3.2
echo "2 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 2 -x OMP_NUM_THREADS=1 ./$TARGET 401 10000000 1e-4 9 >> test/data.txt

# RUN 3.3
echo "4 CORES and 1 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=1 ./$TARGET 401 10000000 1e-4 10 >> test/data.txt

# RUN 3.4
echo "4 CORES and 2 THREADS" >> test/data.txt
mpiexec -np 4 -x OMP_NUM_THREADS=2 ./$TARGET 401 10000000 1e-4 11 >> test/data.txt
echo "=================================" >> test/data.txt

echo "TESTS COMPLETED, see 'test' folder for results"