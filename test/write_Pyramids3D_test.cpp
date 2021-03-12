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

TEST_CASE( "write_Pyramids3D_Test" )
{
    std::vector<double> points 
    {
        0.0, 0.0, 0.0,    0.0, 3.0, 0.0,    1.0, 2.0, 2.0, // 0, 1, 2
        1.0, 3.0,-2.0,   -2.0, 2.0, 0.0,   -1.0, 1.0, 2.0, // 3, 4, 5
        2.0,-2.0,-2.0,    2.0,-2.0, 2.0,   -2.0,-2.0, 2.0, // 6, 7, 8
       -2.0,-2.0,-2.0                                      // 9
    };

    std::vector<VtkIndexType> connectivity 
    {
         5,  0,  1,  2,     // 0
         2,  0,  1,  3,     // 1
         3,  0,  1,  4,     // 2
         4,  0,  1,  5,     // 3
         8,  7,  6,  9,  0  // 4  --> Pyramid
    };

    std::vector<VtkCellType> types { 10, 10, 10, 10, 14 };
    std::vector<VtkIndexType> offsets { 4, 8, 12, 16, 21 };
    
    Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    std::vector<double> flashStrengthPoints { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
    std::vector<double> cellColour { 1.0, 2.0, 3.0, 4.0, 0.0 };

    std::vector<DataSet> pointData { DataSet { std::string( "Flash Strength Points" ), 1, flashStrengthPoints } };
    std::vector<DataSet> cellData { DataSet { std::string( "cell Colour" ), 1, cellColour } };

    std::string filename = "testfiles/pyramids_3D/test.vtu";
    std::string expectedpath = "testfiles/pyramids_3D/";

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

    //The file base64appended.vtu still cannot be opened within ParaView!!!
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

} // write_Pyramids3D_Test"

} // namespace vtu11
