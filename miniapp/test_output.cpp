#include <cmath>
#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <array>
#include "SparseMatrix.hpp"
#include "parallel_io.hpp"

inline void print_global_matrix(const SpKernels::COOMatrix<real_t, idx_t> &mat, int rank, bool mat_notation=true) {
  // add 0 to indexes if you want to print it with mathematical notations
  int offset = mat_notation ? 1 : 0;
  size_t nr_values = mat.nnz;
  // std::cout << to_string(rank) + " " + to_string(nr_values) + "\n";
  for (size_t i = 0; i < nr_values; i++) {
    std::string out_string = to_string(offset + mat.ltgR[mat.ii[i]]) + " "  
                           + to_string(offset + mat.ltgC[mat.jj[i]]) + " "
                           + to_string(mat.vv[i]) + "\n";
    std::cout << out_string;
    
  }
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD, xycomm, zcomm;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    std::string filename;
    if (argc == 2) {
        filename = argv[1];
    } else {
        if (rank == 0) {
            printf("Usage: %s filename\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    std::string::size_type const p(filename.find_last_of('.'));
    std::string mtxName = filename.substr(0, p);
    mtxName = mtxName.substr(mtxName.find_last_of("/\\") +1);

    std::vector<int> rpvec, cpvec;
    // 1D
    std::array<int, 3> dims = {0,0,0};
    std::array<int,3> zeroArr ={0,0,0};
    std::array<int,3> tdims ={0,0,0};

    MPI_Dims_create(size, 3, dims.data());
    MPI_Comm cartcomm;
    MPI_Cart_create(comm, 3, dims.data(), zeroArr.data(), 0, &cartcomm);   

    int X = dims[0], Y = dims[1], Z = dims[2];

    std::array<int, 3> remaindims = {true, true, false};
    MPI_Cart_sub(cartcomm, remaindims.data(), &xycomm); 

    
    int myxyrank;
    MPI_Comm_rank(xycomm, &myxyrank);  

    remaindims = {false, false, true};
    MPI_Cart_sub(cartcomm, remaindims.data(), &zcomm); 
    SpKernels::COOMatrix<real_t, idx_t> Sloc;
    Sloc.mtxName = mtxName;
    {
        std:: vector<int> rpvec2D, cpvec2D;
        /* distribute C */
        SpKernels::read_bin_parallel_distribute_coo(filename, Sloc, rpvec2D, cpvec2D,
                cartcomm,xycomm, zcomm);
        Sloc.localizeIndices();
        MPI_Comm_rank(zcomm, &Sloc.zrank);
        Sloc.rank = rank;

        // print_global_matrix(Sloc, rank, true);
        SpKernels::write_parallel_coo_bin(Sloc, comm, "data/res_test.bin");
    }

  return 0;
}
