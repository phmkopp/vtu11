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

TEST_CASE( "write_test_icosahedron" )
{
    double phi = 1.61803398875;
    double x_tl = 0.0; // x_tl is the translation value in the x coordinate
    double y_tl = 0.0; // y_tl is the translation value in the y coordinate
    double z_tl = 0.0; // z_tl is the translation value in the z coordinate

    std::vector<double> points
    {
        (  0.0 + x_tl ), (  1.0 + y_tl ), (  phi + z_tl ), // 0
        (  0.0 + x_tl ), (  1.0 + y_tl ), ( -phi + z_tl ), // 1
        (  0.0 + x_tl ), ( -1.0 + y_tl ), (  phi + z_tl ), // 2
        (  0.0 + x_tl ), ( -1.0 + y_tl ), ( -phi + z_tl ), // 3
        (  1.0 + x_tl ), (  phi + y_tl ), (  0.0 + z_tl ), // 4
        (  1.0 + x_tl ), ( -phi + y_tl ), (  0.0 + z_tl ), // 5
        ( -1.0 + x_tl ), (  phi + y_tl ), (  0.0 + z_tl ), // 6
        ( -1.0 + x_tl ), ( -phi + y_tl ), (  0.0 + z_tl ), // 7
        (  phi + x_tl ), (  0.0 + y_tl ), (  1.0 + z_tl ), // 8
        (  phi + x_tl ), (  0.0 + y_tl ), ( -1.0 + z_tl ), // 9
        ( -phi + x_tl ), (  0.0 + y_tl ), (  1.0 + z_tl ), // 10
        ( -phi + x_tl ), (  0.0 + y_tl ), ( -1.0 + z_tl ), // 11
        (  0.0 + x_tl ), (  0.0 + y_tl ), (  0.0 + z_tl ), // 12
        (  0.0 + x_tl ), (  0.0 + y_tl ), ( -phi + z_tl ), // 13
        ( -1.0 + x_tl ), ( -1.0 + y_tl ), ( -4.0 + z_tl ), // 14
        (  1.0 + x_tl ), (  1.0 + y_tl ), ( -4.0 + z_tl ), // 15
        (  1.0 + x_tl ), ( -1.0 + y_tl ), ( -4.0 + z_tl ), // 16
        ( -1.0 + x_tl ), (  1.0 + y_tl ), ( -4.0 + z_tl )  // 17
    };

    std::vector<VtkIndexType> connectivity
    {
       0,  2,  8,  12,    // 0
       0,  2,  10, 12,    // 1
       0,  4,  6,  12,    // 2
       0,  4,  8,  12,    // 3
       0,  6,  10, 12,    // 4
       1,  3,  9,  12,    // 5
       1,  3,  11, 12,    // 6
       1,  4,  6,  12,    // 7
       1,  4,  9,  12,    // 8
       1,  6,  11, 12,    // 9
       2,  5,  8,  12,    // 10
       2,  7,  5,  12,    // 11
       2,  7,  10, 12,    // 12
       3,  5,  7,  12,    // 13
       3,  5,  9,  12,    // 14
       3,  7,  11, 12,    // 15
       4,  8,  9,  12,    // 16
       5,  8,  9,  12,    // 17
       6,  10, 11, 12,    // 18
       7,  10, 11, 12,    // 19
       17, 15, 16, 14, 13 // 20 Pyramid
    };

    std::vector<VtkIndexType> offsets
    {
         4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44,
        48, 52, 56, 60, 64, 68, 72, 76, 80, 85
    };

    std::vector<VtkCellType> types
    {
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10, 10, 14
    };

    Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    std::vector<DataSetInfo> dataSetInfo
    {
        { "Point_Data_1", DataSetType::PointData, 1 },
        { "Point_Data_2", DataSetType::PointData, 1 },
        { "Cell_Height_1", DataSetType::CellData, 1 },
        { "Cell_Height_2", DataSetType::CellData, 1 }
    };

    std::vector<double> pointData1
    {
        100.0, 100.0, 100.0, 100.0, 0.0, 0.0, 0.0, 0.0, -100.0, -100.0,
        -100.0, -100.0, 0.0, -100.0, 0.0, 0.0, 0.0, 0.0, 0.0
    };

    std::vector<double> pointData2
    {
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 0.0, 1.0, -1.0, -1.0, -1.0, -1.0
    };

    std::vector<double> cellHeight1
    {
        -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0,
        -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 0
    };

    std::vector<double> cellHeight2
    {
        1.0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 0
    };

    std::vector<DataSetData> dataSetData { pointData1, pointData2, cellHeight1, cellHeight2 };

    std::string filename = "testfiles/icosahedron_3D/test.vtu";
    std::string expectedpath = "testfiles/icosahedron_3D/";

    // The files assume that, we need to add a big endian version
    REQUIRE( endianness( ) == "LittleEndian" );

    SECTION( "ascii_3D" )
    {
        REQUIRE_NOTHROW( writeVtu( filename, mesh, dataSetInfo, dataSetData, "Ascii" ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "ascii.vtu" );

        CHECK( written == expected );
    }

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

} // write_test_icosahedron

} // namespace vtu11
