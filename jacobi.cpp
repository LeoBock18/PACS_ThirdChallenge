#include"jacobi.hpp"

using Matrix = la::dense_matrix;

namespace jacobi{

Matrix solve(std::size_t n, std::function< Real (Real_vec) > f, Real tol, std::size_t n_max, std::function< Real (Real_vec) > dir_bc)
{
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Evaluate subintervals length
    double h = 1./(n-1);

    // Split number of rows (remainers among first processes)
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

    // Boundary conditions along y and force matrix
    for(std::size_t i = 0; i < local_rows; ++i)
    {
        // local to global row index
        std::size_t curr_row = displ[rank]/n + i;
        // Boundary conditions allocation
        local_mat(i,0) = dir_bc({x[curr_row],x[0]});
        local_mat(i,n-1) = dir_bc({x[curr_row],x[n-1]});
        // Force matrix allocation
        for(std::size_t j = 0; j < n; ++j)
        {
            local_f(i,j) = f({x[curr_row],x[j]});
        }
    }

    // Boundary Conditions along x (only first and last rank)
    if(rank == 0)
    {
        // Indicate which row of boundary conditions we are considering
        //std::size_t boundary_row = (rank == 0) ? 0 : local_rows-1;
        //double local_to_global = (rank == 0) ? 0 : 1;
        for(std::size_t j = 1; j < n-1; ++j)
        {
            local_mat(0,j) = dir_bc({x[0],x[j]});
        }
    }

    if(rank == size-1)
    {
        for(std::size_t j = 1; j < n-1; ++j)
        {
            local_mat(local_rows-1,j) = dir_bc({x[n-1],x[j]});
        }
    }   

    MPI_Barrier(MPI_COMM_WORLD);

    // Initialize loop flag variables
    std::size_t n_iter{0};
    bool all_stop = false;
    // Initialize matrix for loop implementation
    Matrix new_mat = local_mat;
    // Jacobi algorithm loop
    while( !all_stop and n_iter < n_max )
    {
        // Initialization of vectors for message passing
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
        

        // Jacobi iteration (common to all ranks), with hybridization
#pragma omp parallel for shared(new_mat)
        for(std::size_t i = 1; i < local_rows-1; ++i)
        {
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(i,j) = 0.25 * (local_mat(i-1,j) + local_mat(i+1,j) + local_mat(i,j-1) + local_mat(i,j+1) + h*h*local_f(i,j));
            }
        }
        // Jacobi iteration (adding upper row), with hybridization
        if(rank != 0)
        {
#pragma omp parallel for shared(new_mat)
            for(std::size_t j = 1; j < n-1; ++j)
            {
                new_mat(0,j) = 0.25 * (upper_row[j] + local_mat(1,j) + local_mat(0,j-1) + local_mat(0,j+1) + h*h*local_f(0,j));
            }
        }
        // Jacobi iteration (adding lower row), with hybridization
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
        // If internal ranks, also first and last rows to be considered
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
        // If first or last ranks, do not consider boundary rows
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
            {
                std::cout << "Number of iterations= " << n_iter;
            }
        }
        // Increase number of iteration
        ++n_iter;
    }
    
    // Build global matrix
    Matrix res(n,n);
    MPI_Gatherv(local_mat.data(), local_rows*n, MPI_DOUBLE, res.data(), recv_counts.data(), displ.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Return the result
    return res;
}

} // end of namespace jacobi