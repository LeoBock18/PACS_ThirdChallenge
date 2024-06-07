/**
 * @file main.cpp
 * @author Leonardo Bocchieri (leonardo.bocchieri@mail.polimi.it)
 * @brief main for solving laplacian problem with Dirichlet boundary conditions using Jacobi algorithm
 * @date 2024-06-06
 * 
 */

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
#include"jacobi.hpp"
#include"densemat.hpp"
#include"chrono.hpp"
#include"writeVTK.hpp"
#include "json.hpp"
#include "muParserInterface.hpp"

// Using namespaces
using json = nlohmann::json;
using namespace MuParserInterface;
using Matrix = la::dense_matrix;
using Real = double;
using Real_vec = std::array<Real,2>;

/**
 * @brief main program for computing numerical solution of Laplace problem
 * 
 * @note Parameters are taken from data.json file if not written on the command line during execution
 */
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

    // Initialize json and muparser variables
    std::ifstream f("data.json");
    json data = json::parse(f);
    muParserInterface fun;
    muParserInterface dir_bc;
    muParserInterface u_ex;

    // Read data from data.json
    std::size_t n = data.value("n", 11);
    std::size_t max_it =  data.value("max_it",500);
    double tol = data.value("tol", 1e-4);
    int file_number = 0; //Default file number for vtk file name
    std::string funString = data.value("fun","");
    std::string dirString = data.value("dir_bc","");
    std::string uexString = data.value("u_ex","");

    // Define functions with muparser method
    fun.set_expression(funString);
    dir_bc.set_expression(dirString);
    u_ex.set_expression(uexString);

    // If parameters given from command line, overwrite json ones
    if (argc == 5) {
        n = std::stoul(argv[1]); // Convert to std::size_t using std::stoul
        max_it = std::stoul(argv[2]); // Convert to std::size_t using std::stoul
        tol = std::atof(argv[3]);
        file_number = std::stoi(argv[4]);
    }
    // Handling case in which just essential parameters are passed (ie no file_number selected)
    if (argc == 4) {
        n = std::stoul(argv[1]);
        max_it = std::stoul(argv[2]);
        tol = std::atof(argv[3]);
    }
    
    // Use chrono utilities for evaluating time for computation
    Timings::Chrono clock;
    clock.start();
    // Compute result
    Matrix res = jacobi::solve(n, fun, tol, max_it, dir_bc);
    clock.stop();

    // Compute L2-error of numerical solution (only rank 0)
    double h = 1./(n-1);
    if(rank == 0)
    {
        // If exact solution provided, compute L2 error of numerical solution
        if( !uexString.empty() ){
            Real err_ex{0};
            for(std::size_t i = 0; i < n; ++i)
            {
                for(std::size_t j = 0; j < n; ++j)
                {
                    err_ex += (res(i,j) - u_ex(M_PI,h*i,h*j)) * (res(i,j) - u_ex(M_PI,h*i,h*j));
                }
            }
            err_ex *= h;
            err_ex = sqrt(err_ex);
            // Print error
            std::cout << "\nError: " << err_ex << std::endl;
        }
        // Print computing time
        std::cout << clock << std::endl;
        std::cout << std::endl;

        // Save numerical result in vtk file
        generateVTKFile("test/out" + std::to_string(file_number) + ".vtk", res, n, h);
    }

    MPI_Finalize();
    return 0;
}