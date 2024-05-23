#include <iostream>
#include <mpi.h>
#include <string>
#include <vector>
#include <array>
#include "SparseMatrix.hpp"
#include "parallel_io.hpp"
#include "distribute.hpp"

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD, xycomm, zcomm;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    std::string filename;
    if (argc > 1) {
      filename = argv[1];
    } else {
      std::cout << "wrong usage, use with mpirun ./build/test_output {input_data_file.bin}" << std::endl;
    }
    
    std::string::size_type const p(filename.find_last_of('.'));
    std::string mtxName = filename.substr(0, p);
    mtxName = mtxName.substr(mtxName.find_last_of("/\\") +1);

    std::vector<int> rpvec, cpvec;
    // 1D
    std::array<int, 3> dims = {0,0,1};
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
        /* distribute Yloc and Xloc  */
        //distribute3D_AB_random(rpvec2D, cpvec2D, rpvec, cpvec, Cloc, f, xycomm);
        SpKernels::distribute3D_AB_respect_communication(rpvec2D, cpvec2D,
                rpvec, cpvec,
                Sloc, xycomm, zcomm, cartcomm);
        Sloc.printMatrix();
    }

  return 0;
}
