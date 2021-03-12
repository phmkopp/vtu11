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

    std::vector<double> cell_1 { 1.0, 2.0 };
    std::vector<double> cell_2 { 10.0, 20.0 };

    std::vector<DataSet> pointData
    {
      DataSet { std::string( "Point_Data_1" ), 1, pointData1 },
      DataSet { std::string( "Point_Data_2" ), 1, pointData2 }
    };

    std::vector<DataSet> cellData
    {
      DataSet { std::string( "Cell_1" ), 1, cell_1 },
      DataSet { std::string( "Cell_2" ), 1, cell_2 }
    };

    std::string filename = "testfiles/hexas_3D/test.vtu";
    std::string expectedpath = "testfiles/hexas_3D/";

    SECTION( "ascii_3D" )
    {
        REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "ascii.vtu" );

        CHECK( written == expected );
    }

    // The files assume that, we need to add a big endian version
    REQUIRE( endianness( ) == "LittleEndian" );

    SECTION( "base64_3D" )
    {
        Base64BinaryWriter writer;

        REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64.vtu" );

        CHECK( written == expected );
    }
    SECTION( "base64appended_3D" )
    {
        Base64BinaryAppendedWriter writer;

        REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "base64appended.vtu" );

        CHECK( written == expected );
    }
    SECTION( "raw_3D" )
    {
        RawBinaryAppendedWriter writer;

        REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw.vtu" );

        CHECK( written == expected );
    }

    #ifdef VTU11_ENABLE_ZLIB
    SECTION( "raw_compressed" )
    {
        CompressedRawBinaryAppendedWriter writer;

        REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );

        auto written = vtu11testing::readFile( filename );
        auto expected = vtu11testing::readFile( expectedpath + "raw_compressed.vtu" );

        CHECK( written == expected );
    }
    #endif

} // hexahedras_test

} // namespace vtu11
