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

// a better solution would be to provide a dedicated main to catch2, like is also done in CoSimIO
struct MPI_RAII
{
    MPI_RAII()
    {
        int argc = 0;
        char** argv = nullptr;
        MPI_Init(&argc, &argv);
    }

    ~MPI_RAII()
    {
        MPI_Finalize();
    }

};

TEST_CASE( "mpi_io_test_binary" )
{
    MPI_RAII mpi;

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

} // mpi_io_test_binary

TEST_CASE( "mpi_io_test_ascii" )
{
    MPI_RAII mpi;

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

} // mpi_io_test_ascii

TEST_CASE( "mpi_output" )
{
    MPI_RAII mpi;
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    StringStringMap headerAttributes { { "byte_order",  endianness( ) },
                                       { "version"   ,  "0.1"         } };

    const std::string filename = "mpi_output.txt";

    vtu11::MPIOutput output_mpi( filename, MPI_COMM_WORLD );

    CHECK(output_mpi.size() == 0);

    {
        ScopedRZO<vtu11::MPIOutput> rzo(output_mpi);

        output_mpi << "This is supposed to be the file header\n";

        if (rank == 0) {
            const std::string original_buffer = output_mpi.buffer();
            CHECK(output_mpi.size() > 0);
            CHECK(output_mpi.size() == original_buffer.size());
            CHECK(output_mpi.buffer() == original_buffer); // make sure the size function does not modify the buffer
            // TODO but how to check the position, is it even required to check it?
        } else {
            CHECK(output_mpi.size() == 0);
            CHECK(output_mpi.buffer().empty());
        }
    }

    // setting the rank as in ScopedRZO causes a flush
    CHECK(output_mpi.size() == 0);
    CHECK(output_mpi.buffer().empty());

    output_mpi << "Data1 r " << rank << "\n";
    output_mpi << "Data2 r " << rank << "\n";
    output_mpi << "Data3 r " << rank << "\n";

    CHECK(output_mpi.size() > 0);
    CHECK(!output_mpi.buffer().empty());

    {
        ScopedRZO<vtu11::MPIOutput> rzo(output_mpi);

        output_mpi << "More output\n";
        output_mpi << "Even More output\n";
    }

    output_mpi << "DataXXX4 r " << rank << "\n";
    output_mpi << "DataXXX5 r " << rank << "\n";
    output_mpi << "DataXXX6 r " << rank << "\n";

    {
        ScopedRZO<vtu11::MPIOutput> rzo(output_mpi);

        output_mpi << "Now add some appended data\n";
    }
    output_mpi << "GarbageData r " << rank << "\n";

    output_mpi.close();

    CHECK(output_mpi.size() == 0);
    CHECK(output_mpi.buffer().empty());

} // mpi_io_test_ascii

} // namespace vtu11
