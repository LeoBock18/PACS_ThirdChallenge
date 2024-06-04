#include"jacobi.hpp"
#include"densemat.hpp"

using Matrix = la::dense_matrix;

namespace jacobi{

Matrix solve(std::size_t n, std::function< Real (Real_vec) > f, Real tol, std::size_t n_max)
{
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double h = 1./(n-1);
    // Split number of rows (remainers among first processes)
    //int local_rows = (n % size < rank) ? n/size : n/size+1;

    // Evaluate number of local elements stored by each process and displacement (remainers among first processes)
    int local_rows = (n % size <= rank) ? n/size : n/size+1;
    std::vector<int> recv_counts(size,0), displ(size,0);
    int start_idx{0};
    for(std::size_t i = 0; i < size; ++i)
    {
        recv_counts[i] = (n % size <= i) ? n/size*n : (n/size+1)*n;
        displ[i] = start_idx;
        start_idx += recv_counts[i];
    }
    
    // Construct local matrices and rhs
    Matrix local_mat(local_rows, n, 0.);
    Matrix local_f(local_rows, n, 0.);

    // Construct domain nodes (1D)
    std::vector<Real> x(n, 0.);
    for(std::size_t i = 0; i < n; ++i)
    {
        x[i] = h*i;
    }

    // Force matrix (put it in other loop if extra of dirichlet)
    for(std::size_t i = 0; i < local_rows; ++i)
    {
        for(std::size_t j = 0; j < n; ++j)
        {
            std::size_t curr_row = displ[rank]/n + i;
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
    Matrix new_mat = local_mat;
    while( !all_stop and n_iter < n_max )
    {
        // Adjacent rows passing
        // Initialization
        std::vector<Real> upper_row(n,0);
        std::vector<Real> lower_row(n,0);

        // Sending lower row
        if(rank != size-1)
        {
            MPI_Send(local_mat.extract_row(local_rows-1).data(), n, MPI_DOUBLE, rank+1, rank, MPI_COMM_WORLD);
        }
        // Receiving lower row (from rank pov, it is upper row)
        if(rank != 0)
        {
            MPI_Recv(upper_row.data(), n, MPI_DOUBLE, rank-1, rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        // Sending upper row
        if(rank != 0)
        {
            MPI_Send(local_mat.extract_row(0).data(), n, MPI_DOUBLE, rank-1, rank, MPI_COMM_WORLD);
        }
        // Receiving upper row (from rank pov, it is lower row)
        if(rank != size-1)
        {
            MPI_Recv(lower_row.data(), n, MPI_DOUBLE, rank+1, rank+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        

        // Jacobi iteration (common to all ranks)
#pragma omp parallel for shared(new_mat)
        for(std::size_t i = 1; i < local_rows-1; ++i)
        {
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(i,j) = 0.25 * (local_mat(i-1,j) + local_mat(i+1,j) + local_mat(i,j-1) + local_mat(i,j+1) + h*h*local_f(i,j));
            }
        }
        // Jacobi iteration (adding upper row)
        if(rank != 0)
        {
#pragma omp parallel for shared(new_mat)
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(0,j) = 0.25 * (upper_row[j] + local_mat(1,j) + local_mat(0,j-1) + local_mat(0,j+1) + h*h*local_f(0,j));
            }
        }
        // Jacobi iteration (adding lower row)
        if(rank != size-1)
        {
#pragma omp parallel for shared(new_mat)
            for(std::size_t j = 1; j < n-1; ++j)
            {
                std::size_t curr_row = local_rows-1;
                new_mat(curr_row,j) =  0.25 * (local_mat(curr_row-1,j) + lower_row[j] + local_mat(curr_row,j-1) + local_mat(curr_row,j+1) + h*h*local_f(curr_row,j));
            }
        }

        // Compute local error
        Real err{0.};
        if(rank != 0 and rank != size-1)
        {
            for(std::size_t i = 0; i < local_rows; ++i)
            {
                for(std::size_t j = 1; j < n-1; ++j)
                {
                    err += (new_mat(i,j) - local_mat(i,j)) * (new_mat(i,j) - local_mat(i,j));
                }
            }
            err *= h;
            err = sqrt(err);
        }
        else if(rank == 0)
        {
            for(std::size_t i = 1; i < local_rows; ++i)
            {
                for(std::size_t j = 1; j < n-1; ++j)
                {
                    err += (new_mat(i,j) - local_mat(i,j)) * (new_mat(i,j) - local_mat(i,j));
                }
            }
            err *= h;
            err = sqrt(err);
        }
        else if(rank == size-1)
        {
            for(std::size_t i = 0; i < local_rows-1; ++i)
            {
                for(std::size_t j = 1; j < n-1; ++j)
                {
                    err += (new_mat(i,j) - local_mat(i,j)) * (new_mat(i,j) - local_mat(i,j));
                }
            }
            err *= h;
            err = sqrt(err);
        }

        // Upgrade local matrix
        la::swap(local_mat,new_mat);
        
        // Pick the max among the errors, change flag if all errors below tolerance
        MPI_Allreduce(MPI_IN_PLACE, &err, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        if(err < tol)
        {
            all_stop = true;
            if(rank == 0)
                std::cout << n_iter << std::endl;
        }
        // Increase number of iteration
        ++n_iter;
    }
    
    // Build global matrix
    Matrix res(n,n);
    MPI_Gatherv(local_mat.data(), local_rows*n, MPI_DOUBLE, res.data(), recv_counts.data(), displ.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    return res;
}

} // end of namespace jacobi