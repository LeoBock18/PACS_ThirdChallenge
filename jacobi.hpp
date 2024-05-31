#ifndef PARALLEL_JACOBI_METHOD_HPP
#define PARALLEL_JACOBI_METHOD_HPP

#include<iostream>
#include"Matrix.hpp"
//#include<mpi.h>

namespace jacobi
{

using Real = double;

apsc::LinearAlgebra::Matrix<Real,ORDERING::ROWMAJOR> solve();
        

} // namespace jacobi

#endif // PARALLEL_JACOBI_METHOD_HPP