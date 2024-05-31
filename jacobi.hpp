#ifndef PARALLEL_JACOBI_METHOD_HPP
#define PARALLEL_JACOBI_METHOD_HPP

#include<iostream>
#include"MyMat0.hpp"
#include<mpi.h>

namespace jacobi
{

using Real = double;
using Real_vec = std::array<Real,2>;

LinearAlgebra::MyMat0<Real, ROWMAJOR> solve(int, std::function< Real (Real_vec) >, Real, std::size_t);
        

} // namespace jacobi

#endif // PARALLEL_JACOBI_METHOD_HPP