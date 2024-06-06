# ThirdChallenge
In this repository the 2D-Laplace equation with Dirichlet boundary conditions is solved using the **Jacobi iteration method** on a uniform Cartesian grid. Since this problem is computationally demanding, the code performs parallel computing using **hybrid programming**, in particular with MPI and OpenMP.

## Files
The project consists of:
- `jacobi.hpp` and `jacobi.hpp`: file with algorithm implementation, consisting on a single function called **solve** which take as input problem parameters and returns resulting matrix
- `densemat.hpp` and `densemat.cpp`: file with matrix class implementation, taken from exercitation lectures of the course ***Algorithms and Parallel Computing 2023-24***
- `main.cpp`: main function for testing the algorithm and for timing computation
- `Doxyfile`: for doxygenating the code
- `Makefile`: create executable just typing `make` (`make clean` and `make distclean` also available, with their canonical meaning), execute with default parameters typing `make run`
- `test/`: folder containing `mytest.sh` and `mydata.txt` (see **Reproducibility** paragraph for additional notions)

## Doxygen
Code is documented using Doxygen. For exploiting it, run
```bash
doxygen Doxyfile
```
and then open `/doc/html/index.html` file on your favourite browser.

## Reproducibility

## Discussion of the results