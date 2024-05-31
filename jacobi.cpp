#include"jacobi.hpp"
#include<functional>
#include<vector>

using Matrix = apsc::LinearAlgebra::Matrix<jacobi::Real,ORDERING::ROWMAJOR>;

Matrix jacobi::solve(int n, std::function< Real (Real_vec) > f)
{
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Real h = 1./(n-1);
    // Split number of rows (remainers among first processes)
    //int local_rows = (n % size < rank) ? n/size : n/size+1;

    // Evaluate number of local elements stored by each process and displacement (remainers among first processes)
    std::vector<int> recv_counts(size,0), displ(size,0);
    int start_idx{0};
    for(std::size_t i = 0; i < size; ++i)
    {
        recv_counts[i] = (n % size < i) ? n/size : n/size+1;
        displ[i] = start_idx;
        start_idx += recv_counts[i];
    }
    
    // Construct local matrices
    Matrix local_mat(recv_counts[rank], n, 0);

    // Construct domain nodes (1D)
    std::vector<Real> x(n);
    for(std::size_t i = 0; i < n; ++i)
    {
        x[i] = h*i;
    }

    // Boundary conditions along y
    for(std::size_t i = 0; i < recv_counts[rank]; ++i)
    {
        std::size_t curr_row = displ[rank] + i;
        local_mat[i,0] = f({x[curr_row],x[0]});
        local_mat[i,n-1] = f({x[curr_row],x[n-1]});
    }

    // Boundary Conditions along x
    if(rank == 0 or rank == size)
    {
        std::size_t boundary_row = (rank == 0) ? 0 : local_rows-1;
        for(std:size_t j = 1; j < n-1; ++j)
        {
            local_mat[boundary_row,j] = f({x[boundary_row],x[j]});
        }
    }

    MPI_Barrier




}