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
	namespace parallelHelper
	{
		TEST_CASE("getAmountOfCells_test")
		{
			size_t numberOfFiles, numberOfCells;
			std::array<size_t, 2> result;
			SECTION("numberOfCellsDivisible")
			{
				numberOfFiles = 4;
				numberOfCells = 12;
				result = { 3,4 };
				CHECK(GetAmountOfCells(&numberOfFiles, numberOfCells) == result);
			}
			SECTION("numberOfCells<numberOfFiles")
			{
				numberOfFiles = 4;
				numberOfCells = 3;
				result = { 1,3 };
				CHECK(GetAmountOfCells(&numberOfFiles, numberOfCells) == result);
				CHECK(numberOfFiles == numberOfCells);
			}
			SECTION("numberOfCells>numberOfFiles")
			{
				numberOfFiles = 4;
				numberOfCells = 6;
				result = { 2,2 };
				CHECK(GetAmountOfCells(&numberOfFiles, numberOfCells) == result);
			}
			SECTION("checkThrow")
			{
				numberOfFiles = 0;
				numberOfCells = 6;
				REQUIRE_THROWS(GetAmountOfCells(&numberOfFiles, numberOfCells));
			}
		}
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
				DataSet{std::string("Flash Strength Points"), 1, flashStrengthPoints },
				DataSet{std::string("pointData2"), 1, pointData2 }
			};
			std::vector<DataSet> cellData{
				DataSet{std::string("cell Colour"), 1, cellColour },
				DataSet{std::string("cellData2"),1, cellData2 }
			};
			size_t numberOfFiles = 3;
			std::array<size_t, 2> cellDistribution=parallelHelper::GetAmountOfCells(&numberOfFiles, mesh.numberOfCells());
			//std::tuple<Vtu11UnstructuredMesh, std::vector<DataSet>, std::vector<DataSet>> pieceData;
			SECTION("Piece1")
			{
				size_t fileId = 1;
				std::tuple<Vtu11UnstructuredMesh, std::vector<DataSet>, std::vector<DataSet>> pieceData{ GetCurrentDataSet(mesh, pointData, cellData, cellDistribution, fileId) };
				CHECK(1==1);
			}
		}
	}//namespace parallelHelper
}//namespace vtu11

