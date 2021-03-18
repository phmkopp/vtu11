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
namespace partitioneddata
{

struct MeshData
{
    std::vector<double> points;
    std::vector<VtkIndexType> connectivity;
    std::vector<VtkIndexType> offsets;
    std::vector<VtkCellType> types;

    Vtu11UnstructuredMesh mesh( )
    {
        return { points, connectivity, offsets, types };
    }
};

std::vector<DataSetInfo> dataSetInfo( )
{
    return { { "Flash Strength Points", DataSetType::PointData, 1 },
             { "pointData2"           , DataSetType::PointData, 1 },
             { "cell Colour"          , DataSetType::CellData, 1 },
             { "cellData2"           , DataSetType::CellData, 1 } };
}

/* 
 * Unpartitioned data:
 * 
 * std::vector<double> points
 * {
 *      0.0,  0.0,  0.0, // 0
 *      0.0,  3.0,  0.0, // 1
 *      1.0,  2.0,  2.0, // 2
 *      1.0,  3.0, -2.0, // 3
 *     -2.0,  2.0,  0.0, // 4
 *     -1.0,  1.0,  2.0, // 5
 *      2.0, -2.0, -2.0, // 6
 *      2.0, -2.0,  2.0, // 7
 *     -2.0, -2.0,  2.0, // 8
 *     -2.0, -2.0, -2.0  // 9
 * };
 * 
 * std::vector<VtkIndexType> connectivity
 * {
 *     5,  0,  1,  2,     // 0
 *     2,  0,  1,  3,     // 1
 *     3,  0,  1,  4,     // 2
 *     4,  0,  1,  5,     // 3
 *     8,  7,  6,  9,  0  // 4  --> Pyramid
 * };
 * 
 * std::vector<VtkCellType> types { 10, 10, 10, 10, 14 };
 * std::vector<VtkIndexType> offsets { 4, 8, 12, 16, 21 };
 *
 * std::vector<double> flashStrengthPoints { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
 * std::vector<double> pointData2 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };
 * std::vector<double> cellColour { 1.0, 2.0, 3.0, 4.0, 0.0 };
 * std::vector<double> cellData2 { 5.0, 6.0, 7.0, 8.0, 9.0 };
 * 
 */

std::pair<MeshData, std::vector<DataSetData>> partition( size_t index )
{
    MeshData meshData;
    DataSetData flashStength, pointData2, cellColour, cellData2;

    if( index == 0 )
    {
        meshData = MeshData
        {
            // Points
            { -1.0,  1.0,  2.0,
               0.0,  0.0,  0.0, 
               0.0,  3.0,  0.0,
               1.0,  2.0,  2.0,
               1.0,  3.0, -2.0 },
        
            // Connectivity
            { 0, 1, 2, 3, 
              3, 1, 2, 4 },

            // Offsets 
            { 4, 8 },

            // Types
            { 10, 10 }
        };

        flashStength = { 1.0, 1.0, 1.0, 1.0, 1.0 };
        pointData2 = { 5.0, 0.0, 1.0, 2.0, 3.0 };
        cellColour = { 1.0, 2.0 };
        cellData2 = { 5.0, 6.0 };
    }

    if( index == 1 )
    {
        meshData = MeshData
        {
            // Points
            {  1.0,  3.0, -2.0, 
               0.0,  0.0,  0.0, 
               0.0,  3.0,  0.0, 
              -2.0,  2.0,  0.0, 
              -1.0,  1.0,  2.0  },
        
            // Connectivity
            { 0, 1, 2, 3, 
              3, 1, 2, 4 },

            // Offsets 
            { 4, 8 },

            // Types
            { 10, 10 }
        };

        flashStength = { 1.0, 1.0, 1.0, 1.0, 1.0 };
        pointData2 = { 3.0, 0.0, 1.0, 4.0, 5.0 };
        cellColour = { 3.0, 4.0 };
        cellData2 = { 7.0, 8.0 };
    }
    
    if( index == 2 )
    {
        meshData = MeshData
        {
            // Points
            { -2.0, -2.0,  2.0, 
               2.0, -2.0,  2.0, 
               2.0, -2.0, -2.0, 
              -2.0, -2.0, -2.0, 
               0.0,  0.0,  0.0 },
        
            // Connectivity
            { 0, 1, 2, 3, 4 },

            // Offsets 
            { 5 },

            // Types
            { 14 }
        };

        flashStength = { 0.0, 0.0, 0.0, 0.0, 1.0 };
        pointData2 = { 8.0, 7.0, 6.0, 9.0, 0.0 };
        cellColour = { 0.0 };
        cellData2 = { 9.0 };
    }

    return { meshData, { flashStength, pointData2, cellColour, cellData2 } };
}

} // namespace partitioneddata

TEST_CASE( "pyramids3D_test" )
{
    std::string writeDir = "testfiles/parallel_write/pyramids_3D/tester/";
    std::string expectedDir = "testfiles/parallel_write/pyramids_3D/";

    std::string basename = "pyramids3D_parallel_test";
    std::string piecebase = basename + "/" + basename + "_";

    auto dataSetInfo = partitioneddata::dataSetInfo( );

    size_t numberOfFiles = 3;
    
    // Targets for std::tie
    partitioneddata::MeshData meshData;
    std::vector<DataSetData> dataSetData;

    SECTION( "ascii" )
    {
        writePVtu( writeDir, basename, dataSetInfo, numberOfFiles );

        // Create all pieces and the .pvtu file and check the pieces for correctness
        for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
        {
            std::tie( meshData, dataSetData ) = partitioneddata::partition( fileId );

            auto mesh = meshData.mesh( );

            writePartition( writeDir, basename, mesh, dataSetInfo, dataSetData, fileId, "Ascii" );

            std::string piecename = piecebase + std::to_string( fileId ) + ".vtu";

            auto written = vtu11testing::readFile( writeDir + piecename );
            auto expected = vtu11testing::readFile( expectedDir + "ascii/" + piecename );

            CHECK( written == expected );
        }

        // Check the .pvtu file
        auto written = vtu11testing::readFile( writeDir + basename + ".pvtu" );
        auto expected = vtu11testing::readFile( expectedDir + "ascii/" + basename + ".pvtu" );

        CHECK( written == expected );
    }

    SECTION( "base64" )
    {
        writePVtu( writeDir, basename, dataSetInfo, numberOfFiles );

        for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
        {
            std::tie( meshData, dataSetData ) = partitioneddata::partition( fileId );

            auto mesh = meshData.mesh( );

            writePartition( writeDir, basename, mesh, dataSetInfo, dataSetData, fileId, "Base64Inline" );

            std::string piecename = piecebase + std::to_string( fileId ) + ".vtu";

            auto written = vtu11testing::readFile( writeDir + piecename );
            auto expected = vtu11testing::readFile( expectedDir + "base64/" + piecename );

            CHECK( written == expected );
        }

        // Check the .pvtu file
        auto written = vtu11testing::readFile( writeDir + basename + ".pvtu" );
        auto expected = vtu11testing::readFile( expectedDir + "base64/" + basename + ".pvtu" );

        CHECK( written == expected );
    }

    SECTION( "base64appended" )
    {
        writePVtu( writeDir, basename, dataSetInfo, numberOfFiles );

        for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
        {
            std::tie( meshData, dataSetData ) = partitioneddata::partition( fileId );

            auto mesh = meshData.mesh( );

            writePartition( writeDir, basename, mesh, dataSetInfo, dataSetData, fileId, "Base64Appended" );

            std::string piecename = piecebase + std::to_string( fileId ) + ".vtu";

            auto written = vtu11testing::readFile( writeDir + piecename );
            auto expected = vtu11testing::readFile( expectedDir + "base64appended/" + piecename );

            CHECK( written == expected );
        }

        // Check the .pvtu file
        auto written = vtu11testing::readFile( writeDir + basename + ".pvtu" );
        auto expected = vtu11testing::readFile( expectedDir + "base64appended/" + basename + ".pvtu" );

        CHECK( written == expected );
    }

    SECTION( "raw" )
    {
        writePVtu( writeDir, basename, dataSetInfo, numberOfFiles );

        for( size_t fileId = 0; fileId < numberOfFiles; fileId++ )
        {
            std::tie( meshData, dataSetData ) = partitioneddata::partition( fileId );
            
            auto mesh = meshData.mesh( );

            writePartition( writeDir, basename, mesh, dataSetInfo, dataSetData, fileId, "RawBinary" );

            std::string piecename = piecebase + std::to_string( fileId ) + ".vtu";

            auto written = vtu11testing::readFile( writeDir + piecename );
            auto expected = vtu11testing::readFile( expectedDir + "raw/" + piecename );

            CHECK( written == expected );
        }

        // Check the .pvtu file
        auto written = vtu11testing::readFile( writeDir + basename + ".pvtu" );
        auto expected = vtu11testing::readFile( expectedDir + "raw/" + basename + ".pvtu" );

        CHECK( written == expected );
    }

} // pyramids3D_test

} // namespace vtu11
