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

TEST_CASE( "mpi_io_test_binary" )
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
    MPI_File_open(MPI_COMM_WORLD, "mpi_io_test_binary.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_handle);

    MPI_Offset offset = static_cast<MPI_Offset>(rank * static_cast<int>(sizeof(int)));
    MPI_File_write_at(file_handle, offset, &rank, 1, MPI_INT, MPI_STATUS_IGNORE);

    MPI_File_close(&file_handle);

    MPI_Finalize();
} // mpi_io_test_binary

TEST_CASE( "mpi_io_test_ascii" )
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
    MPI_File_open(MPI_COMM_WORLD, "mpi_io_test_ascii.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file_handle);

    const std::string my_data = std::to_string(rank) + " ";

    int offset;
    int my_size = my_data.size();
    MPI_Scan(&my_size, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_File_write_at(file_handle, offset-my_size, my_data.data(), my_data.size(), MPI_CHAR, MPI_STATUS_IGNORE);

    MPI_File_close(&file_handle);

    MPI_Finalize();
} // mpi_io_test_ascii

TEST_CASE( "mpi_io_scoped_xml_tag" )
{
    int argc = 0;
    char** argv = nullptr;
    MPI_Init(&argc, &argv);

    StringStringMap headerAttributes { { "byte_order",  endianness( ) },
                                       { "version"   ,  "0.1"         } };

    const std::string filename = "mpi_io_scoped_xml_tag.txt";

    vtu11::MPIOutput output_mpi( filename, MPI_COMM_WORLD );

    {
        ScopedRZO<vtu11::MPIOutput> rzo(output_mpi);

        output_mpi << "This is supposed to be the file header\n";
    }

    output_mpi << "Data1\n";
    output_mpi << "Data2\n";
    output_mpi << "Data3\n";

    output_mpi.close();

    MPI_Finalize();
} // mpi_io_test_ascii

} // namespace vtu11
