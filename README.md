# ThirdChallenge
## Overview
In this repository the 2D-Laplace equation with Dirichlet boundary conditions is solved using the **Jacobi iteration method** on a uniform Cartesian grid. Since this problem is computationally demanding, the code performs parallel computing using **hybrid programming**, in particular with MPI and OpenMP.

## Files
The project consists of:
- `jacobi.hpp` and `jacobi.hpp`: file with algorithm implementation, consisting on a single function called **solve** which take as input problem parameters and returns resulting matrix
- `densemat.hpp` and `densemat.cpp`: file with matrix class implementation, taken from exercitation lectures of the course ***Algorithms and Parallel Computing 2023-24***
- `writeVTK.hpp`: file for exporting solution and grid coordinates in vtk format, open it in **ParaView**
- `main.cpp`: main function for testing the algorithm and for timing computation
- `data.json`: file for input parameters and functions
- `Makefile`: create executable just typing `make` (`make clean` and `make distclean` also available, with their canonical meaning), execute with default parameters typing `make run`
- `test/`: folder containing `mytest.sh`, for reproducing my tests, and `mydata.txt`, file with my results (see **Reproducibility** paragraph for additional notions)
- `Doxyfile`: for doxygenating the code

## Doxygen
Code is documented using Doxygen. For exploiting it, run
```bash
doxygen Doxyfile
```
and then open `/doc/html/index.html` file on your favourite browser.

## Reproducibility
There are two ways of running this code:
- **using json parameters**: modify parameters in the json file, then run with `make run`. Results should appear on the command line, while solutions will be saved in `test/out0.vtk`. Please notice that number of cores and ranks should be changed in the Makefile (line 26)
- **using bash script**: if you want to reproduce the exact same tests I made, you need to run the bash script file `mytest.sh` contained in `test` folder. This will output a file of results `test/data.txt`, which you can compare with `test/mydata.txt`, and a list of vtk files. For doing so, run
```bash
chmod +x test/mytests.sh
./test/mytests.sh
```

## Remarks
The whole process was made possible by the fact that parameters (number of nodes, max iterations, tolerance) are taken from json file by default, but they are overwritten if you write them directly on the command line while executing.
Please notice that problem functions are always taken from json file, but you are not forced to input an **exact solution**: in this case, no L2-error would be computed.
Even though we are dealing with a 2D-problem, functions have 3 input variables: that is because the first variable (namely `t`) is used in the code for computing the exact value of **pi**.

## Discussion of the results
In `test/mydata.txt` you can find the results I obtained running tests on my computer. I have made 3 different tests, changing the dimension of the grid (n=51, 101, 401 respectively) and setting a proper max_it and tolerance. Every of the 3 tests was made using 1,2,4 cores. The results follow what I expected: with a low number of nodes, the speed-up was not so pronounced, whereas going with high grid dimensions we can noticed an important difference in efficiency (the last test was not performed in a serialized manner due to very high inefficiency). The same happens when introducing hybridization: only with test3 we can see a speed-up.