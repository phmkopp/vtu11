#include "catch.hpp"
#include "vtu11.hpp"

#include <sstream>
#include <iostream> // REMOVE
#include <fstream>

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
     5,  9, 10, 11, // 4
     6, 10, 11,  7  // 5
  };
   
  std::vector<size_t> offsets { 0, 4, 8, 12, 16, 20 };
  std::vector<unsigned short> types { 9, 9, 9, 9, 9, 9 };
   
  UnstructuredMesh mesh{ points, connectivity, offsets, types };
   
  std::vector<double> pointData1 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
  std::vector<double> pointData2 { 0.1, -0.2, 0.3, -0.4, 0.5, 0.6, -0.7, 0.8, 0.9, 1.0, 1.1 };
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
  
  REQUIRE_NOTHROW( write( output, mesh, pointData, cellData ) );

  std::cout << output.str( ) << std::endl;

  std::ofstream file;
  file.open( "result.vtu" );
  write( file, mesh, pointData, cellData );
  file.close( );
}

} // namespace vtu11

