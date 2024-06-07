/**
 * @file jacobi.hpp
 * @author Leonardo Bocchieri (leonardo.bocchieri@mail.polimi.it)
 * @brief Implementation of jacobi algorithm for solving laplacian PDE
 * @date 2024-06-06
 * 
 */

#ifndef PARALLEL_JACOBI_METHOD_HPP
#define PARALLEL_JACOBI_METHOD_HPP

#include<iostream>
#include<array>
#include<functional>
#include<vector>
#include<cmath>
#include<mpi.h>
#include<omp.h>
#include"densemat.hpp"

//! Everything inside namespace jacobi
namespace jacobi
{

/**
 * @note Using Real for generalization
 * 
 */
using Real = double;
using Real_vec = std::array<Real,2>;

/**
 * @brief 
 * 
 * @param n number of cores for parallel implementation
 * @param f right hand side given function
 * @param tol algorithm tolerance
 * @param n_max algorithm maximum number of iterations
 * @param dir_bc dirichlet boundary conditions function
 * @return matrix of solution in Cartesian grid
 */
la::dense_matrix solve(std::size_t n, std::function< Real (Real, Real, Real) > f, Real tol, std::size_t n_max, std::function< Real (Real, Real, Real) > dir_bc);
        

} // namespace jacobi

#endif // PARALLEL_JACOBI_METHOD_HPP