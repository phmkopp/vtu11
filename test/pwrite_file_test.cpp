//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#include "vtu11_testing.hpp"
#include "vtu11.hpp"

namespace vtu11
{

// Tests, if parallelWrite is running without error. Successful
// and correct creation of .vtu files is not tested here
TEST_CASE( "pvtu_simple_test" )
{
    std::vector<double> points
    {
        0.0, 0.0, 0.5,    0.0, 0.3, 0.5,    0.0, 0.7, 0.5,    0.0, 1.0, 0.5, // 0,  1,  2,  3
        0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
        1.0, 0.0, 0.5,    1.0, 0.3, 0.5,    1.0, 0.7, 0.5,    1.0, 1.0, 0.5  // 8,  9, 10, 11
    };

    std::vector<VtkIndexType> connectivity
    {
       0,  4,  5,  1, // 0
       1,  5,  6,  2, // 1
       2,  6,  7,  3, // 2
       4,  8,  9,  5, // 3
       5,  9, 10,  6, // 4
       6, 10, 11,  7  // 5
    };

    std::vector<VtkIndexType> offsets { 4, 8, 12, 16, 20, 24 };
    std::vector<VtkCellType> types { 9, 9, 9, 9, 9, 9 };

    Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    std::vector<double> pointData1 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
    std::vector<double> pointData2 { 0.1, -0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, 0.9, 1.0, 1.1, -1.2 };
    std::vector<double> cellData1 { 3.2, 4.3, 5.4, 6.5, 7.6, 8.7 };
    std::vector<double> cellData2 { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
    std::vector<double> cellData3 = cellData1;

    std::vector<DataSetInfo> dataSetInfo
    {
         { "pointData1", DataSetType::PointData, 1 },
         { "pointData2", DataSetType::PointData, 1 },
         { "cellData1",  DataSetType::CellData, 1 },
         { "cellData2",  DataSetType::CellData, 1 },
         { "cellData3",  DataSetType::CellData, 1 }
    };

    std::vector<DataSetData> dataSetData 
    { 
        pointData1, pointData2, cellData1, cellData2, cellData3 
    };

    std::string filename = "parallel_write_test.pvtu";
    std::string basename = "parallel_write_test";

    vtu11fs::path path = "testfiles/parallel_write/pwrite_tester/";
    vtu11fs::create_directory( path );

    REQUIRE_NOTHROW( writePVtu( path.string( ), basename, dataSetInfo, 2 ) );
    REQUIRE_NOTHROW( writePartition( path.string( ), basename, mesh, dataSetInfo, dataSetData, 0 ) );

    REQUIRE( endianness( ) == "LittleEndian" );

} // write_parallel_file_test

} // namespace vtu11
