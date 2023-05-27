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

} // mpi_output

TEST_CASE( "mpi_output_scoped_xml_tag" )
{
    MPI_RAII mpi;
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    StringStringMap headerAttributes { { "byte_order",  endianness( ) },
                                       { "version"   ,  "0.1"         } };

    const std::string filename = "mpi_output_scoped_xml_tag.txt";

    vtu11::MPIOutput output_mpi( filename, MPI_COMM_WORLD );

    CHECK(output_mpi.size() == 0);

    {
        ScopedRZO<vtu11::MPIOutput> rzo(output_mpi);
        output_mpi << "<?xml version=\"1.0\"?>\n";
    }

    {
        ScopedXmlTag<vtu11::MPIOutput> vtkFileTag( output_mpi, "VTKFile", headerAttributes );

        // if (rank == 0) {
        //     const std::string original_buffer = output_mpi.buffer();
        //     CHECK(output_mpi.size() > 0);
        //     CHECK(output_mpi.size() == original_buffer.size());
        //     CHECK(output_mpi.buffer() == original_buffer); // make sure the size function does not modify the buffer
        //     // TODO but how to check the position, is it even required to check it?
        // } else {
        //     CHECK(output_mpi.size() == 0);
        //     CHECK(output_mpi.buffer().empty());
        // }

        // setting the rank as in ScopedRZO causes a flush
        // CHECK(output_mpi.size() == 0);
        // CHECK(output_mpi.buffer().empty());

        output_mpi << "Data1 r " << rank << "\n";
        output_mpi << "Data2 r " << rank << "\n";
        output_mpi << "Data3 r " << rank << "\n";

        // CHECK(output_mpi.size() > 0);
        // CHECK(!output_mpi.buffer().empty());

        {
            ScopedXmlTag<vtu11::MPIOutput> gridTag( output_mpi, "UnstructuredGrid", { } );

            output_mpi << "DataXXX4 r " << rank << "\n";
            output_mpi << "DataXXX5 r " << rank << "\n";
            output_mpi << "DataXXX6 r " << rank << "\n";

            {
                ScopedXmlTag<vtu11::MPIOutput> nodesTag( output_mpi, "Nodes", { } );
                output_mpi << "NodeInfo r " << rank << "\n";
            }

            {
                ScopedXmlTag<vtu11::MPIOutput> elementsTag( output_mpi, "Elements", { } );
                output_mpi << "ElementInfo r " << rank << " ";
            }

        }


        // CHECK(output_mpi.size() == 0);
        // CHECK(output_mpi.buffer().empty());
    }

    output_mpi.close();

} // mpi_output_scoped_xml_tag

TEST_CASE( "mpi_hexahedras_test" )
{
    MPI_RAII mpi;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double drank = rank;

    std::vector<double> points
    {
        0.0, 0.0, 0.0,    5.0, 0.0, 0.0,    0.0, 5.0, 0.0,    5.0, 5.0, 0.0, //0, 1, 2, 3
        0.0, 0.0, 5.0,    5.0, 0.0, 5.0,    0.0, 5.0, 5.0,    5.0, 5.0, 5.0, //4, 5, 6, 7
        2.0, 2.0, 5.0,    7.0, 2.0, 5.0,    2.0, 7.0, 5.0,    7.0, 7.0, 5.0, //8, 9, 10, 11
        2.0, 2.0, 10.0,   7.0, 2.0, 10.0,   2.0, 7.0, 10.0,   7.0, 7.0, 10.0 //12, 13, 14, 15
    };

    // displace for MPI
    double offset = drank * 10;
    for (auto& c : points) {
        c += offset;
    }

    std::vector<VtkIndexType> connectivity
    {
       0, 1, 2,  3,  4,  5,  6,  7, // 0
       8, 9, 10, 11, 12, 13, 14, 15 // 1, hexahedra - cubes
    };

    for (auto& c : connectivity) {
        c += 16*rank;
    }

    std::vector<VtkCellType> types { 11, 11 };
    std::vector<VtkIndexType> offsets { 8, 16 };

    offsets[0] = 16*rank + 8;
    offsets[1] = offsets[0] + 8;

    DistributedVtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    std::vector<DataSetInfo> dataSetInfo
    {
        { "Point_Data_1", DataSetType::PointData, 1 },
        { "Point_Data_2", DataSetType::PointData, 1 },
        { "Cell_1", DataSetType::CellData, 1 },
        { "Cell_2", DataSetType::CellData, 1 }
        ,{ "MPI_Rank", DataSetType::CellData, 1 }
    };

    std::vector<double> pointData1
    {
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 5.0, 5.0, 5.0
    };

    std::vector<double> pointData2
    {
        41.0, 13.0, 16.0, 81.0, 51.0, 31.0, 18.0, 12.0,
        19.0, 21.0, 11.0, 19.0, 16.0, 45.0, 35.0, 58.0
    };

    std::vector<double> cellData1 { 1.0, 2.0 };
    std::vector<double> cellData2 { 10.0, 20.0 };

    std::vector<double> cellDataMPI { static_cast<double>(rank), static_cast<double>(rank) };

    std::vector<DataSetData> dataSetData { pointData1, pointData2, cellData1, cellData2, cellDataMPI };

    std::string filename = "mpi_hexahedras_test.vtu";
    // std::string expectedpath = "testfiles/hexas_3D/";

    SECTION( "Base64Inline" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, MPI_COMM_WORLD, "base64inline" ) ); // base64inline

        // auto written = vtu11testing::readFile( filename );
        // auto expected = vtu11testing::readFile( expectedpath + "ascii.vtu" );

        // CHECK( written == expected );
    }

} // mpi_output_scoped_xml_tag

} // namespace vtu11
