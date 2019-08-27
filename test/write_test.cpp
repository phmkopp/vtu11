#include "catch.hpp"
#include "vtu11.hpp"

#include <sstream>
#include <fstream>
#include <iostream> // remove

namespace vtu11
{

TEST_CASE("writeAscii_test")
{

  std::vector<double> points
  {
      0.0, 0.0, 0.5,    0.0, 0.3, 0.5,    0.0, 0.7, 0.5,    0.0, 1.0, 0.5, // 0,  1,  2,  3
      0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
      1.0, 0.0, 0.5,    1.0, 0.3, 0.5,    1.0, 0.7, 0.5,    1.0, 1.0, 0.5  // 8,  9, 10, 11
  };
   
  std::vector<size_t> connectivity
  {
     0,  4,  5,  1, // 0
     1,  5,  6,  2, // 1
     2,  6,  7,  3, // 2
     4,  8,  9,  5, // 3
     5,  9, 10,  6, // 4
     6, 10, 11,  7  // 5
  };
   
  std::vector<size_t> offsets { 4, 8, 12, 16, 20, 24 };
  std::vector<VtkCellType> types { 9, 9, 9, 9, 9, 9 };
   
  UnstructuredMesh mesh{ points, connectivity, offsets, types };
   
  std::vector<double> pointData1 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
  std::vector<double> pointData2 { 0.1, -0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, 0.9, 1.0, 1.1, -1.2 };
  std::vector<double> cellData1 { 3.2, 4.3, 5.4, 6.5, 7.6, 8.7 };
  std::vector<double> cellData2 { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0  };
  std::vector<double> cellData3 = cellData1;
  
  std::vector<DataSet> pointData
  {
    { std::string( "pointData1" ), 1, pointData1 },
    { std::string( "pointData2" ), 1, pointData2 },
  };

  std::vector<DataSet> cellData
  {
    { std::string( "cellData1" ), 1, cellData1 },
    { std::string( "cellData2" ), 1, cellData2 },
    { std::string( "cellData3" ), 1, cellData3 }
  };
  
  std::ostringstream output;

  auto readFile = []( const std::string& filename )
  {
    std::ifstream file( filename );
    
    std::string contents, str;
    
    while( std::getline( file, str ) )
    {
      contents += str + "\n";
    }  
    
    file.close( );
    
    return contents;
  };

  SECTION( "ascii" )
  {
    REQUIRE_NOTHROW( write<AsciiWriter>( output, mesh, pointData, cellData ) );
    
    auto expected = readFile( "testfiles/ascii_2x3.vtu" );
  
    CHECK( output.str( ) == expected );
  }
  
  // The files assume that, we need to add a big endian version
  REQUIRE( endianness( ) == "LittleEndian" );

  SECTION( "base64" )
  {
    REQUIRE_NOTHROW( write<Base64BinaryWriter>( output, mesh, pointData, cellData ) );
    
    auto expected = readFile( "testfiles/base64_2x3.vtu" );
  
    CHECK( output.str( ) == expected );
  }

  SECTION( "base64appended" )
  {
    REQUIRE_NOTHROW( write<Base64BinaryAppendedWriter>( output, mesh, pointData, cellData ) );

    auto expected = readFile( "testfiles/base64appended_2x3.vtu" );

    CHECK( output.str( ) == expected );
  }

  SECTION( "raw" )
  {
    REQUIRE_NOTHROW( write<RawBinaryAppendedWriter>( output, mesh, pointData, cellData ) );

    auto expected = readFile( "testfiles/raw_2x3.vtu" );

    CHECK( output.str( ) == expected );
  }
}

// write raw binary:
//
// std::ofstream file;
// file.open( "raw_test.vtu" );
// write<RawBinaryAppendedWriter>( file, mesh, pointData, cellData );
// file.close( );

} // namespace vtu11

