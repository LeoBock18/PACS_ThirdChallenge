#include<iostream>
#include<mpi.h>
#include<cmath>
#include"jacobi.hpp"
#include"densemat.hpp"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank==0)
    {
        jacobi::Real pi = M_PI;
        std::size_t n_max = 1000;
        int n = 4;
        jacobi::Real tol = 1e-3;
        std::function< jacobi::Real (jacobi::Real_vec) > f = [pi](auto const & x){return 8*pi*pi*std::sin(2*pi*x[0])*std::sin(2*pi*x[1]);};

        la::dense_matrix res = jacobi::solve(n, f, tol, n_max);

        std::cout << "Matrix:\n" << res;
    }

    MPI_Finalize();
    return 0;
}