#include"jacobi.hpp"
#include<functional>
#include<vector>

using Matrix = LinearAlgebra::MyMat0<Real, ROWMAJOR>;

Matrix jacobi::solve(int n, std::function< Real (Real_vec) > f, Real tol, std::size_t n_max)
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
    
    // Construct local matrices and rhs
    Matrix local_mat(recv_counts[rank], n, 0);
    Matrix local_f(recv_counts[rank],n);

    // Construct domain nodes (1D)
    std::vector<Real> x(n);
    for(std::size_t i = 0; i < n; ++i)
    {
        x[i] = h*i;
    }

    // Force matrix (put it in other loop if extra of dirichlet)
    for(std::size_t i = 0; i < recv_counts[rank]; ++i)
    {
        for(std::size_t j = 0; j< n; ++j)
        {
            std::size_t curr_row = displ[rank] + i;
            local_f(i,j) = f({x[curr_row],x[j]});
        }
    }

/*
    // Boundary conditions along y
    for(std::size_t i = 0; i < recv_counts[rank]; ++i)
    {
        std::size_t curr_row = displ[rank] + i;
        local_mat(i,0) = g_dir({x[curr_row],x[0]});
        local_mat(i,n-1) = g_dir({x[curr_row],x[n-1]});
    }

    // Boundary Conditions along x (only first and last rank)
    if(rank == 0 or rank == size)
    {
        std::size_t boundary_row = (rank == 0) ? 0 : local_rows-1;
        for(std:size_t j = 1; j < n-1; ++j)
        {
            local_mat(boundary_row,j) = g_dir({x[boundary_row],x[j]});
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
*/
    
    std::size_t n_iter{0};
    bool all_stop = false;
    Matrix new_mat(recv_counts[rank], n, 0);
    while( !all_stop and n_iter < n_max )
    {
        // Adjacent rows passing
        // Initialization
        std::vector<Real> upper_row(n,0);
        std::vector<Real> lower_row(n,0);

        // Sending lower row
        if(rank != size-1)
        {
            MPI_Send(local_mat.row(recv_counts[rank]-1).data(), n, MPI_DOUBLE, rank+1, rank, MPI_COMM_WORLD);
        }
        // Receiving lower row (from rank pov, it is upper row)
        if(rank != 0)
        {
            MPI_Recv(upper_row.data(), n, MPI_DOUBLE, rank-1, rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORED);
        }

        // Sending upper row
        if(rank != 0)
        {
            MPI_Send(local_mat.row(0).data(), n, MPI_DOUBLE, rank-1, rank, MPI_COMM_WORLD);
        }
        // Receiving upper row (from rank pov, it is lower row)
        if(rank != size-1)
        {
            MPI_Recv(lower_row.data(), n, MPI_DOUBLE, rank+1, rank+1, MPI_COMM_WORLD, MPI_STATUS_IGNORED);
        }

        // Jacobi iteration (common to all ranks)
        for(std::size_t i = 1; i < recv_counts[rank]-1; ++i)
        {
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(i,j) += 0.25 * (local_mat(i-1,j) + local_mat(i+1,j) + local_mat(i,j-1) + local_mat(i,j+1) + h*h*local_f(i,j));
            }
        }
        // Jacobi iteration (adding upper row)
        if(rank != 0)
        {
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(0,j) += 0.25 * upper_row[j];
            }
        }
        // Jacobi iteration (adding lower row)
        if(rank != size-1)
        {
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(recv_counts[rank]-1,j) += 0.25 * lower_row[j];
            }
        }

    }



}