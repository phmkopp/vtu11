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
#include "inc/parallel_helper.hpp"

#include <sstream>
#include <fstream>
namespace vtu11
{
struct Vtu11AllData
{
  std::vector<double> points_;
  std::vector<VtkIndexType> connectivity_;
  std::vector<VtkIndexType> offsets_;
  std::vector<VtkCellType> types_;
  std::vector<DataSet> pointData_;
  std::vector<DataSet> cellData_;

  std::vector<double>& points( ) { return points_; }
  std::vector<VtkIndexType>& connectivity() { return connectivity_; }
  std::vector<VtkIndexType>& offsets( ) { return offsets_; }
  std::vector<VtkCellType>& types( ) { return types_; }
  std::vector<DataSet>& pointData( ) { return pointData_; }
  std::vector<DataSet>& cellData( ) { return cellData_; }
  size_t numberOfPoints( ) { return points_.size( ) / 3; }
  size_t numberOfCells( ) { return types_.size( ); }
};
namespace parallelHelper
{
namespace distributeData
{
  //This function adjusts the numberOfFiles, if needed and returns:
  // 1. The amount of cells per file-piece
  // 2. The fileId of that file-piece, that has the highest id with the returned amount of cells. All pieces with a higher file-id have one cell less!!
  //    (only, if there are at least one cell for each requested piece. Otherwise the numberOfFiles gets adjusted!)
  std::array<size_t, 2> GetAmountOfCells( size_t * numberOfFiles, size_t numberOfCells )
  {
    size_t cellsPerFile = 1;
    //Check if the number of cells can be distributed equally to each file-piece
    if( *numberOfFiles == 0 )
    {
      throw std::runtime_error( "Determining the amount of cells per file-piece went wrong!" );
    }
    else if( numberOfCells % *numberOfFiles == 0 )
    {
      cellsPerFile = numberOfCells / *numberOfFiles;
      return { cellsPerFile , *numberOfFiles };
    }
    //If there are less cells then numberOfFiles, the numberOfFiles-Variable needs to be adjusted,
    //so that each file-piece contains all data, that concerns exactly one cell
    else if ( floor( numberOfCells / double( *numberOfFiles ) ) == 0. )
    {
      *numberOfFiles = numberOfCells;
      return { cellsPerFile,*numberOfFiles };
    }
    //If the numberOfCells is not divisible by the number of Files, there are "cellsPerFile" cells in the first file-pieces
    //and "cellsPerFile - 1" cells in all pieces, with the fileId > lastFileWithCellsPerFile
    else if ( floor( numberOfCells / *numberOfFiles ) > 0. )
    {
      cellsPerFile = ceil( numberOfCells / double( *numberOfFiles ) );
      size_t lastFileWithCellsPerFile = numberOfCells % *numberOfFiles - 1;
      return { cellsPerFile, lastFileWithCellsPerFile };
    }
    else
    {
      throw std::runtime_error( "Determining the amount of cells per file-piece went wrong!" );
    }

  } // GetAmountOfCells
  
    //This function returns the cell and point datasets only for one specific piece
    std::array<std::vector<DataSet>, 2> GetCurrentCellPointData( const std::vector<DataSet> & pointDataGlobal,
                                                                      const std::vector<DataSet> & cellDataGlobal,
                                                                      std::vector<VtkIndexType> & globalTranslation,
                                                                      size_t firstCellId, size_t lastCellId )
  {
    std::vector<DataSet> pointData, cellData;
    for( DataSet pointSet : pointDataGlobal )
    {
      std::vector<double> data;
      for( VtkIndexType pointId : globalTranslation )
      {
        data.push_back( std::get<2>( pointSet )[pointId] );
      }
      pointData.push_back( DataSet{ std::get<0>( pointSet ),std::get<1>( pointSet ),data } );
    }
    for( DataSet cellSet : cellDataGlobal )
    {
      std::vector<double> data;
      for( size_t cellId = firstCellId; cellId < lastCellId; ++cellId )
      {
        data.push_back( std::get<2>( cellSet )[cellId] );
      }
      cellData.push_back( DataSet{ std::get<0>( cellSet ),std::get<1>( cellSet ),data } );
    }
    return { pointData, cellData };
  } // GetCurrentCellPointData






//This function distributes the global mesh into equal small pieces and returns those mesh-pieces and the associated data
  //There is no fileId 0, it starts with fileId==1
  template<typename MeshGenerator, typename AllMeshData>
  inline AllMeshData GetCurrentDataSet( MeshGenerator& mesh,
                                        const std::vector<DataSet>& pointData,
                                        const std::vector<DataSet>& cellData,
                                        std::array<size_t, 2> cellDistribution,
                                        size_t fileId )
  {
    std::vector<double> points;
    std::vector<VtkIndexType> relevantConnectivity, connectivity, offsets;
    std::vector<VtkCellType> types;
    size_t firstCellId, lastCellId;
    if( fileId <= cellDistribution[1] )
    {
      //There is no change in the amount of cells per file
      firstCellId = fileId * cellDistribution[0]; //Id of the first cell in that piece
      lastCellId = fileId * cellDistribution[0] + cellDistribution[0]; //Id of the last cell + 1 in that piece
    }
    else
    {
      //There is a change in the amount of cells per file
      firstCellId = (cellDistribution[1] + 1) * cellDistribution[0] + (fileId - cellDistribution[1] - 1)
                    * (cellDistribution[0] - 1);//Id of the first cell in that piece
      lastCellId = (cellDistribution[1] + 1) * cellDistribution[0] + (fileId - cellDistribution[1])
                   * (cellDistribution[0] - 1);//Id of the last cell + 1 in that piece
    }
    //loop over all cells in that piece to store the new types, new offsets and select the relevant connectivity
    for (size_t currentCell = firstCellId; currentCell < lastCellId; ++currentCell)
    {
      if( firstCellId == 0 )
      {
        offsets.push_back( mesh.offsets( )[currentCell] );
      }
      else
      {
        offsets.push_back( mesh.offsets( )[currentCell] - mesh.offsets( )[firstCellId - 1] );
      }
      types.push_back( mesh.types( )[currentCell] );
      //Determine the number of Points a cell consists of
      size_t numberOfCellPoints;
      if( currentCell == 0 ) { numberOfCellPoints = mesh.offsets( )[currentCell]; }
      else
      {
        numberOfCellPoints = mesh.offsets( )[currentCell] - mesh.offsets( )[currentCell - 1];
      }

      //loop over each point of each cell in that piece to get the connectivity
      for( size_t cellPoint = numberOfCellPoints; cellPoint > 0; --cellPoint )
      {
        size_t actualPointIndex = mesh.connectivity( )[mesh.offsets( )[currentCell] - cellPoint];
        relevantConnectivity.push_back(actualPointIndex);
      }
    }//loop over all cells

    //How to find the points, that are needed in that piece?
    //1. loop over connectivity
    //2. check if the link to this connectivity-point already exists
    // -->if yes, just add the local connectivity to the connectivity piece
    // -->if no,
    //      I. add point to global translation vector, to know, what the original index of this point was
    //      II. add new point coordinates to the local vector
    //      III. add new point index (counter) to the connectivity vector 

    VtkIndexType counter = 0;
    std::vector<VtkIndexType> globalTranslation;
    for( VtkIndexType connection : relevantConnectivity )
    {
      auto piecePointId = std::find( globalTranslation.begin( ), globalTranslation.end( ), connection );
      if( piecePointId == globalTranslation.end( ) )//If it is a new point, add new connectivity, add the global connectivity to the globalTranslation and add the new point to the local points
      {
        connectivity.push_back( counter );
        globalTranslation.push_back( connection );
        //loop over x-, y- and z-coordinate points and store all points in the piece order in piece internal points
        for( size_t coordinate = 0; coordinate < 3; ++coordinate )
        {
          points.push_back( mesh.points( )[connection * 3 + coordinate] );
        }
        counter++;
      }
      else //if the point already exists, just add the new connectivity
      {
        VtkIndexType newConnectivity = std::distance( globalTranslation.begin(), piecePointId );
        connectivity.push_back( newConnectivity );
      }
    }
    std::array < std::vector<DataSet>, 2> pointCellData = GetCurrentCellPointData( pointData, cellData, globalTranslation, firstCellId, lastCellId );
    AllMeshData allDataPiece{ points, connectivity, offsets, types, pointCellData[0], pointCellData[1] };
    return allDataPiece;
  } // GetCurrentDataSet

  
  TEST_CASE( "getAmountOfCells_test" )
  {
    size_t numberOfFiles, numberOfCells;
    std::array<size_t, 2> result;
    SECTION( "numberOfCellsDivisible" )
    {
      numberOfFiles = 4;
      numberOfCells = 12;
      result = { 3,4 };
      CHECK( GetAmountOfCells( &numberOfFiles, numberOfCells ) == result );
    }
    SECTION( "numberOfCells<numberOfFiles" )
    {
      numberOfFiles = 4;
      numberOfCells = 3;
      result = { 1,3 };
      CHECK( GetAmountOfCells( &numberOfFiles, numberOfCells ) == result );
      CHECK( numberOfFiles == numberOfCells );
    }
    SECTION( "numberOfCells>numberOfFiles" )
    {
      numberOfFiles = 4;
      numberOfCells = 6;
      result = { 2,1 };
      CHECK( GetAmountOfCells( &numberOfFiles, numberOfCells ) == result );
    }
    SECTION( "checkThrow" )
    {
      numberOfFiles = 0;
      numberOfCells = 6;
      REQUIRE_THROWS( GetAmountOfCells( &numberOfFiles, numberOfCells ) );
    }
  }
} // namespace distributeData
  TEST_CASE("Distribute_Data_test")
  {
    std::vector<double> points{
    0.0, 0.0, 0.0,    0.0, 3.0, 0.0,    1.0, 2.0, 2.0, //0, 1, 2
    1.0, 3.0,-2.0,   -2.0, 2.0, 0.0,   -1.0, 1.0, 2.0, //3, 4, 5
    2.0,-2.0,-2.0,    2.0,-2.0, 2.0,   -2.0,-2.0, 2.0, //6, 7, 8
    -2.0,-2.0,-2.0                                     //9
    };
    std::vector<VtkIndexType> connectivity{
    5,  0,  1,  2,  //0
    2,  0,  1,  3,  //1
    3,  0,  1,  4,  //2
    4,  0,  1,  5,  //3
    8,  7,  6,  9,  0  //4  --> Pyramid
    };
    std::vector<VtkCellType> types{ 10, 10, 10, 10, 14 };
    std::vector<VtkIndexType> offsets{ 4, 8, 12, 16, 21 };
    Vtu11UnstructuredMesh mesh{ points, connectivity, offsets, types };

    std::vector<double> flashStrengthPoints{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
    std::vector<double> pointData2{ 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
    std::vector<double> cellColour{ 1.0, 2.0, 3.0, 4.0, 0.0 };
    std::vector<double> cellData2{ 5.0, 6.0, 7.0, 8.0, 9.0 };

    std::vector<DataSet> pointData{
      DataSet{ std::string( "Flash Strength Points" ), 1, flashStrengthPoints },
      DataSet{ std::string( "pointData2" ), 1, pointData2 }
    };
    std::vector<DataSet> cellData{
      DataSet{ std::string( "cell Colour" ), 1, cellColour },
      DataSet{ std::string( "cellData2" ),1, cellData2 }
    };
    size_t numberOfFiles = 3;
    std::array<size_t, 2> cellDistribution = parallelHelper::distributeData::GetAmountOfCells( &numberOfFiles, mesh.numberOfCells( ) );
    Vtu11AllData allData{ points, connectivity, offsets, types,pointData,cellData };
    std::string path = "testfiles/parallel_write/pyramids_3D/tester/";
    std::string basename = "pyramids3D_parallel_test";
	std::string timestep = "1";
    auto readFile = []( const std::string& filename )
    {
      std::ifstream file( filename );

      if( !file.is_open( ) ) {
        std::stringstream err_msg;
        err_msg << filename << " could not be opened!";
        throw std::runtime_error( err_msg.str( ) );
      }

      std::string contents, str;

      while( std::getline( file, str ) )
      {
        contents += str + "\n";
      }

      file.close( );

      return contents;
    };
    SECTION( "test_pyramids3D_parallel_ascii" )
    {
      AsciiWriter writer;      
      //create all pieces and the .pvtu file and check the pieces for correctness
      for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
      {

        Vtu11AllData pieceDataSets{ distributeData::GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>( mesh, pointData, cellData, cellDistribution, fileId ) };
        Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points( ),pieceDataSets.connectivity( ),pieceDataSets.offsets( ),pieceDataSets.types( ) };
        parallelWrite( path, basename, timestep, pieceMesh, pieceDataSets.pointData( ), pieceDataSets.cellData( ), fileId, numberOfFiles, writer );
        std::string filename = path + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string( fileId ) + ".vtu";

        auto written = readFile( filename );
        auto expected = readFile( "testfiles/parallel_write/pyramids_3D/ascii/" + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string( fileId ) + ".vtu" );

        CHECK( written == expected );
      }
      //check the .pvtu file
      auto written = readFile( path + basename + "_ts_" + timestep + ".pvtu" );
      auto expected = readFile( "testfiles/parallel_write/pyramids_3D/ascii/" + basename + "_ts_" + timestep + ".pvtu" );

      CHECK( written == expected );
    }
    SECTION( "test_pyramids3D_parallel_base64" )
    {
      Base64BinaryWriter writer;
      for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
      {
        Vtu11AllData pieceDataSets{ distributeData::GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>( mesh, pointData, cellData, cellDistribution, fileId ) };
        Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points( ),pieceDataSets.connectivity( ),pieceDataSets.offsets( ),pieceDataSets.types( ) };
		parallelWrite(path, basename, timestep, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
		std::string filename = path + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu";

		auto written = readFile(filename);
		auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64/" + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu");

		CHECK(written == expected);
	  }
	  //check the .pvtu file
	  auto written = readFile(path + basename + "_ts_" + timestep + ".pvtu");
	  auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64/" + basename + "_ts_" + timestep + ".pvtu");

      CHECK( written == expected );
    }
    SECTION( "test_pyramids3D_parallel_base64appended_3D" )
    {
      Base64BinaryAppendedWriter writer;
      for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
      {
        Vtu11AllData pieceDataSets{ distributeData::GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>(mesh, pointData, cellData, cellDistribution, fileId) };
        Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points( ),pieceDataSets.connectivity( ),pieceDataSets.offsets( ),pieceDataSets.types( ) };
		parallelWrite(path, basename, timestep, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
		std::string filename = path + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu";

		auto written = readFile(filename);
		auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64appended/" + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu");

		CHECK(written == expected);
	  }
	  //check the .pvtu file
	  auto written = readFile(path + basename + "_ts_" + timestep + ".pvtu");
	  auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64appended/" + basename + "_ts_" + timestep + ".pvtu");

      CHECK( written == expected );
    }
    SECTION( "test_pyramids3D_parallel_raw_3D" )
    {
      RawBinaryAppendedWriter writer;
      for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
      {
        Vtu11AllData pieceDataSets{ distributeData::GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>( mesh, pointData, cellData, cellDistribution, fileId ) };
        Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points( ),pieceDataSets.connectivity( ),pieceDataSets.offsets( ),pieceDataSets.types( ) };
		parallelWrite(path, basename, timestep, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
		std::string filename = path + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu";

		auto written = readFile(filename);
		auto expected = readFile("testfiles/parallel_write/pyramids_3D/raw/" + basename + "_ts_" + timestep + "/" + basename + "_ts_" + timestep + "_pid_" + std::to_string(fileId) + ".vtu");

		CHECK(written == expected);
	  }
	  //check the .pvtu file
	  auto written = readFile(path + basename + "_ts_" + timestep + ".pvtu");
	  auto expected = readFile("testfiles/parallel_write/pyramids_3D/raw/" + basename + "_ts_" + timestep + ".pvtu");

      CHECK( written == expected );
    }
  } // TEST_CASE("Distribute_Data_test")

} // namespace parallelHelper
} // namespace vtu11
