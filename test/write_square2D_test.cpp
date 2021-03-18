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

namespace vtu11
{

TEST_CASE( "square2D_test" )
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

    std::vector<DataSetInfo> dataSetInfo
    {
        { "pointData1", DataSetType::PointData, 1 },
        { "pointData2", DataSetType::PointData, 1 },
        { "cellData1", DataSetType::CellData, 1 },
        { "cellData2", DataSetType::CellData, 1 },
        { "cellData3", DataSetType::CellData, 1 }
    };

    std::vector<double> pointData1 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
    std::vector<double> pointData2 { 0.1, -0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, 0.9, 1.0, 1.1, -1.2 };
    std::vector<double> cellData1 { 3.2, 4.3, 5.4, 6.5, 7.6, 8.7 };
    std::vector<double> cellData2 { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
    std::vector<double> cellData3 = cellData1;

    std::vector<DataSetData> dataSetData { pointData1, pointData2, cellData1, cellData2, cellData3 };

    std::string filename = "testfiles/square_2D/test.vtu";
    std::string expectedpath = "testfiles/square_2D/";

    SECTION( "ascii" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "ascii" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "ascii.vtu" );

        CHECK( written == expected );
    }

    // The files assume that, we need to add a big endian version
    REQUIRE( endianness( ) == "LittleEndian" );

    SECTION( "base64" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "base64inline" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64.vtu" );

        CHECK( written == expected );
    }

    SECTION( "base64appended" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "base64appended" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64appended.vtu" );

        CHECK( written == expected );
    }

    SECTION( "raw" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "rawbinary" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw.vtu" );

        CHECK( written == expected );
    }

    #ifdef VTU11_ENABLE_ZLIB
    SECTION( "raw_compressed" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "rawbinarycompressed" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw_compressed.vtu" );

        CHECK( written == expected );
    }
    #endif

    SECTION( "pvtu" )
    {
        std::string basename = "parallel_write_test";

        vtu11fs::path path = "testfiles/parallel_write/pwrite_tester/";
        vtu11fs::create_directory( path );

        REQUIRE_NOTHROW( writePVtu( path.string( ), basename, dataSetInfo, 1 ) );
        REQUIRE_NOTHROW( writePartition( path.string( ), basename, mesh, dataSetInfo, dataSetData, 0 ) );
    }

} // square2D_test

TEST_CASE( "encodedNumberOfBytes_test" )
{
    std::array<size_t, 11> expectedSizes { 0, 4, 4, 4, 8, 8, 8, 12, 12, 12, 16 };

    for( size_t numberOfBytes = 0; numberOfBytes < expectedSizes.size( ); ++numberOfBytes )
    {
        CHECK( encodedNumberOfBytes( numberOfBytes ) == expectedSizes[numberOfBytes] );
    }
}

} // namespace vtu11
