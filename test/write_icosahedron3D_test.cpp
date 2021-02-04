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

#include <sstream>
#include <fstream>
#include <iostream> // remove

namespace vtu11
{
TEST_CASE("write_test_icosahedron")
{
  double phi = 1.61803398875;
  double x_tl = 0.0; // x_tl is the translation value in the x coordinate
  double y_tl = 0.0; // y_tl is the translation value in the y coordinate
  double z_tl = 0.0; // z_tl is the translation value in the z coordinate
  std::vector<double> points
  {
      (0.0 + x_tl), (1.0 + y_tl), (phi + z_tl),     (0.0 + x_tl), (1.0 + y_tl), (-phi + z_tl),  // 0, 1
      (0.0 + x_tl), (-1.0 + y_tl), (phi + z_tl),    (0.0 + x_tl), (-1.0 + y_tl), (-phi + z_tl), // 2, 3,
      (1.0 + x_tl), (phi + y_tl), (0.0 + z_tl),     (1.0 + x_tl), (-phi + y_tl), (0.0 + z_tl),  // 4, 5,
      (-1.0 + x_tl), (phi + y_tl), (0.0 + z_tl),    (-1.0 + x_tl), (-phi + y_tl), (0.0 + z_tl), // 6, 7,
      (phi + x_tl), (0.0 + y_tl), (1.0 + z_tl),     (phi + x_tl), (0.0 + y_tl), (-1.0 + z_tl),  // 8, 9,
      (-phi + x_tl), (0.0 + y_tl), (1.0 + z_tl),    (-phi + x_tl), (0.0 + y_tl), (-1.0 + z_tl), // 10, 11
      (0.0 + x_tl), (0.0 + y_tl), (0.0 + z_tl),                                                 // 12
      (0.0 + x_tl), (0.0 + y_tl), (-phi + z_tl),    (-1.0 + x_tl), (-1.0 + y_tl), (-4.0 + z_tl),//13, 14
      (1.0 + x_tl), (1.0 + y_tl), (-4.0 + z_tl),    (1.0 + x_tl), (-1.0 + y_tl), (-4.0 + z_tl), //15, 16
      (-1.0 + x_tl), (1.0 + y_tl), (-4.0 + z_tl)                                                //17
  };

  std::vector<VtkIndexType> connectivity
  {
     0, 2, 8, 12,  // 0
     0, 2, 10, 12, // 1
     0, 4, 6, 12,  // 2
     0, 4, 8, 12,  // 3
     0, 6, 10, 12, // 4
     1, 3, 9, 12,  // 5
     1, 3, 11, 12, // 6
     1, 4, 6, 12,  // 7
     1, 4, 9, 12,  // 8
     1, 6, 11, 12, // 9
     2, 5, 8, 12,  // 10
     2, 7, 5, 12,  // 11
     2, 7, 10, 12, // 12
     3, 5, 7, 12,  // 13
     3, 5, 9, 12,  // 14
     3, 7, 11, 12, // 15
     4, 8, 9, 12,  // 16
     5, 8, 9, 12,  // 17
     6, 10, 11, 12,// 18
     7, 10, 11, 12,// 19
     17, 15, 16, 14, 13// 20 Pyramid
  };

  std::vector<VtkIndexType> offsets { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 85 };
  std::vector<VtkCellType> types { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 14 };

  Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

  std::vector<double> pointData1 { 100.0, 100.0, 100.0, 100.0, 0.0, 0.0, 0.0, 0.0, -100.0, -100.0, -100.0, -100.0, 0.0, -100.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  std::vector<double> pointData2 { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, -1.0, -1.0, -1.0, -1.0 };
  std::vector<double> cellHeight1 { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 0 };
  std::vector<double> cellHeight2 { 1.0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0 };

  std::vector<DataSet> pointData
  {
    DataSet { std::string( "Point_Data_1" ), 1, pointData1 },
    DataSet { std::string( "Point_Data_2" ), 1, pointData2 }
  };

  std::vector<DataSet> cellData
  {
    DataSet { std::string( "Cell_Height_1" ), 1, cellHeight1 },
    DataSet { std::string( "Cell_Height_2" ), 1, cellHeight2 }
  };

  auto readFile = []( const std::string& filename )
  {
    std::ifstream file( filename );

    if ( !file.is_open() ) {
        std::stringstream err_msg;
        err_msg << filename << " could not be opened!";
        throw std::runtime_error( err_msg.str() );
    }

    std::string contents, str;

    while( std::getline( file, str ) )
    {
	  contents += str + "\n";
    }

    file.close();

    return contents;
  };
  std::string filename = "testfiles/icosahedron_3D/test.vtu";
  // The files assume that, we need to add a big endian version
  REQUIRE(endianness() == "LittleEndian");
  SECTION( "ascii_3D" )
  {
    REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData ));
    auto written = readFile(filename);
    auto expected = readFile("testfiles/icosahedron_3D/ascii.vtu");

    CHECK(written == expected);
  }
  SECTION( "base64_3D" )
  {
    Base64BinaryWriter writer;

    REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );
    auto written = readFile( filename );
    auto expected = readFile( "testfiles/icosahedron_3D/base64.vtu" );

    CHECK( written == expected );
  }
  SECTION( "base64appended_3D" )
  {
    Base64BinaryAppendedWriter writer;

    REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );
    auto written = readFile( filename );
    auto expected = readFile( "testfiles/icosahedron_3D/base64appended.vtu" );

    CHECK( written == expected );
  }
  SECTION( "raw_3D" )
  {
    RawBinaryAppendedWriter writer;

    REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );
    auto written = readFile( filename );
    auto expected = readFile( "testfiles/icosahedron_3D/raw.vtu" );

    CHECK( written == expected );
  }
#ifdef VTU11_ENABLE_ZLIB
  SECTION( "raw_compressed" )
  {
    CompressedRawBinaryAppendedWriter writer;

    REQUIRE_NOTHROW( write( filename, mesh, pointData, cellData, writer ) );

    auto written = readFile( filename );
    auto expected = readFile( "testfiles/icosahedron_3D/raw_compressed.vtu" );

    CHECK( written == expected );
  }
#endif

}

} // namespace vtu11
