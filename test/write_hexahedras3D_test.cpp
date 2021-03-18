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

TEST_CASE( "hexahedras_test" )
{

    std::vector<double> points
    {
        0.0, 0.0, 0.0,    5.0, 0.0, 0.0,    0.0, 5.0, 0.0,    5.0, 5.0, 0.0, //0, 1, 2, 3
        0.0, 0.0, 5.0,    5.0, 0.0, 5.0,    0.0, 5.0, 5.0,    5.0, 5.0, 5.0, //4, 5, 6, 7
        2.0, 2.0, 5.0,    7.0, 2.0, 5.0,    2.0, 7.0, 5.0,    7.0, 7.0, 5.0, //8, 9, 10, 11
        2.0, 2.0, 10.0,   7.0, 2.0, 10.0,   2.0, 7.0, 10.0,   7.0, 7.0, 10.0 //12, 13, 14, 15
    };

    std::vector<VtkIndexType> connectivity
    {
       0, 1, 2,  3,  4,  5,  6,  7, // 0
       8, 9, 10, 11, 12, 13, 14, 15 // 1, hexahedra - cubes
    };

    std::vector<VtkIndexType> offsets { 8, 16 };
    std::vector<VtkCellType> types { 11, 11 };

    Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    std::vector<DataSetInfo> dataSetInfo
    {
        { "Point_Data_1", DataSetType::PointData, 1 },
        { "Point_Data_2", DataSetType::PointData, 1 },
        { "Cell_1", DataSetType::CellData, 1 },
        { "Cell_2", DataSetType::CellData, 1 }
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

    std::vector<DataSetData> dataSetData { pointData1, pointData2, cellData1, cellData2 };

    std::string filename = "testfiles/hexas_3D/test.vtu";
    std::string expectedpath = "testfiles/hexas_3D/";

    SECTION( "ascii_3D" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "Ascii" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "ascii.vtu" );

        CHECK( written == expected );
    }

    // The files assume that, we need to add a big endian version
    REQUIRE( endianness( ) == "LittleEndian" );

    SECTION( "base64_3D" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "Base64Inline" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64.vtu" );

        CHECK( written == expected );
    }

    SECTION( "base64appended_3D" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "Base64Appended" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64appended.vtu" );

        CHECK( written == expected );
    }

    SECTION( "raw_3D" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "RawBinary" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw.vtu" );

        CHECK( written == expected );
    }

    #ifdef VTU11_ENABLE_ZLIB
    SECTION( "raw_compressed" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "RawBinaryCompressed" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw_compressed.vtu" );

        CHECK( written == expected );
    }
    #endif

} // hexahedras_test

} // namespace vtu11
