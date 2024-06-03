#ifndef PARALLEL_JACOBI_METHOD_HPP
#define PARALLEL_JACOBI_METHOD_HPP

#include<iostream>
#include<array>
#include<functional>
#include<vector>
#include<cmath>
#include<mpi.h>
#include"densemat.hpp"

namespace jacobi
{

using Real = double;
using Real_vec = std::array<Real,2>;

la::dense_matrix solve(int, std::function< Real (Real_vec) >, Real, std::size_t);
        

} // namespace jacobi

#endif // PARALLEL_JACOBI_METHOD_HPP