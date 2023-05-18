//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#include "vtu11/vtu11.hpp"
#include "vtu11_testing.hpp"
#include "mpi.h"

namespace vtu11
{

TEST_CASE( "mpi_io_test_1" )
{
    int argc = 0;
    char** argv = nullptr;
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "Hello from rank" << rank << " / " << size << std::endl;


    MPI_File file_handle;
    MPI_File_open(MPI_COMM_WORLD, "mpi_io_test.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_handle);

    int buf[10];
    for (int i=0; i<10; i++) buf[i] = 100 * rank + i;

    /* set file offset for this calling process */
    // MPI_Offset offset = (MPI_Offset)rank * 10 * sizeof(int);
    // MPI_File_set_view(file_handle, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    // MPI_File_write_all(file_handle, &buf[0], 10, MPI_INT, MPI_STATUS_IGNORE);

    // MPI_File_write(file_handle, &buf[0], 10, MPI_INT, MPI_STATUS_IGNORE);


    MPI_File_write_at(file_handle, rank*sizeof(int), &rank, 1, MPI_INT, MPI_STATUS_IGNORE);

    std::cout << "Hello 222 from rank" << rank << " / " << size << std::endl;
    MPI_File_close(&file_handle);

    MPI_Finalize();


} // write_Pyramids3D_Test"

} // namespace vtu11
