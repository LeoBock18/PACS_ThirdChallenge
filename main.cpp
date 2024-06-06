#include<iostream>
#include<array>
#include<functional>
#include<vector>
#include <string>
#include <fstream>
#include<cmath>
#include<mpi.h>
#include<omp.h>
#include<ctime>
#include"jacobi.cpp"
#include"densemat.cpp"
#include"chrono.hpp"
#include"writeVTK.hpp"

using Matrix = la::dense_matrix;
using Real = double;
using Real_vec = std::array<Real,2>;

int main(int argc, char* argv[])
{
    int provided;
    // Initialize MPI (thread because of hybridization)
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    // Check if required support is available
    if(MPI_THREAD_MULTIPLE < provided)
    {
        std::cerr << "The required thread support is not available" << std::endl;
        return 1;
    }

    // Initialize rank and size
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <a> <b> <c>" << std::endl;
        return 1;
    }

    std::size_t n = std::stoul(argv[1]);
    std::size_t max_it = std::stoul(argv[2]); // Convert to std::size_t using std::stoul
    Real tol = std::atof(argv[3]);

/*
    std::ifstream f("data.json");
    json data = json::parse(f);
    muParserInterface func;

    std::string funString = data.value("fun","");
    const unsigned int max_it =  data.value("max_it",500);
    const double n = data.value("n", 11);
    const double tol = data.value("tol", 1e-4);

    func.set_expression(funString);
*/
/*
    std::size_t n = 11;
    std::size_t max_it = 5000;
    Real tol = 1e-5;
    */
    std::function< Real (Real_vec) > f = [](auto const & x){return 8*M_PI*M_PI*std::sin(2*M_PI*x[0])*std::sin(2*M_PI*x[1]);};
    std::function< Real (Real_vec) > u_ex = [](auto const & x){return std::sin(2*M_PI*x[0])*std::sin(2*M_PI*x[1])+x[0]+x[1];};
    std::function< Real (Real_vec) > dir_bc = [](auto const & x){return x[0] + x[1];};
    

    Timings::Chrono clock;
    clock.start();
    Matrix res = jacobi::solve(n, f, tol, max_it, dir_bc);
    clock.stop();

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
        std::cout << clock << std::endl;

        generateVTKFile("out.vtk", res, n, h);
    }

    MPI_Finalize();
    return 0;
}