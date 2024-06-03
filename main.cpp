#include<iostream>
#include<array>
#include<functional>
#include<vector>
#include<cmath>
#include<mpi.h>
#include<ctime>
#include"jacobi.cpp"
#include"densemat.cpp"
//#include"chrono.hpp"

using Matrix = la::dense_matrix;
using Real = double;
using Real_vec = std::array<Real,2>;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Real pi = M_PI;
    std::size_t n_max = 5000;
    std::size_t n = 101;
    Real tol = 1e-5;
    std::function< Real (Real_vec) > f = [pi](auto const & x){return 8*pi*pi*std::sin(2*pi*x[0])*std::sin(2*pi*x[1]);};
    std::function< Real (Real_vec) > u_ex = [pi](auto const & x){return std::sin(2*pi*x[0])*std::sin(2*pi*x[1]);};

    //Timings::Chrono clock;
    //clock.start();
    Matrix res = jacobi::solve(n, f, tol, n_max);
    //clock.stop();

    double h = 1./(n-1);
    if(rank == 0)
    {
        Real err_ex{0};
        for(std::size_t i = 0; i < n; ++i)
        {
            for(std::size_t j = 0; j < n; ++j)
            {
                err_ex += (res(i,j) - u_ex({h*i,h*j})) * (res(i,j) - u_ex({h*i,h*j}));
            }
        }
        err_ex *= h;
        err_ex = sqrt(err_ex);
        std::cout << "Result:\n" << res << std::endl;
        std::cout << "\nError: " << err_ex << std::endl;
        //std::cout << clock << std::endl;
    }

    MPI_Finalize();
    return 0;
}