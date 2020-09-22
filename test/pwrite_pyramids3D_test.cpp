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
			std::array<size_t, 2> cellDistribution = parallelHelper::GetAmountOfCells(&numberOfFiles, mesh.numberOfCells());
			Vtu11AllData allData{ points, connectivity, offsets, types,pointData,cellData };
            std::string path = "testfiles/parallel_write/pyramids_3D/tester/";
			std::string basename = "pyramids3D_parallel_test";
            fs::create_directories(path); //Must create the tester folder on the go for this test.

			auto readFile = [](const std::string& filename)
			{
				std::ifstream file(filename);

				if (!file.is_open()) {
					std::stringstream err_msg;
					err_msg << filename << " could not be opened!";
					throw std::runtime_error(err_msg.str());
				}

				std::string contents, str;

				while (std::getline(file, str))
				{
					contents += str + "\n";
				}

				file.close();

				return contents;
			};
			SECTION("test_pyramids3D_parallel_ascii")
			{
				//create all pieces and the .pvtu file and check the pieces for correctness
				for (size_t fileId = 0; fileId < numberOfFiles; fileId++)
				{
					Vtu11AllData pieceDataSets{ GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>(mesh, pointData, cellData, cellDistribution, fileId) };
					Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points(),pieceDataSets.connectivity(),pieceDataSets.offsets(),pieceDataSets.types() };
					parallelWrite(path, basename, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles);
					std::string filename = path + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu";

					auto written = readFile(filename);
					auto expected = readFile("testfiles/parallel_write/pyramids_3D/ascii/" + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu");

					CHECK(written == expected);
				}
				//check the .pvtu file
				auto written = readFile(path + basename + ".pvtu");
				auto expected = readFile("testfiles/parallel_write/pyramids_3D/ascii/" + basename + ".pvtu");

				CHECK(written == expected);
			}
			SECTION("test_pyramids3D_parallel_base64")
			{
				Base64BinaryWriter writer;
				for (size_t fileId = 0; fileId < numberOfFiles; fileId++)
				{
					Vtu11AllData pieceDataSets{ GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>(mesh, pointData, cellData, cellDistribution, fileId) };
					Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points(),pieceDataSets.connectivity(),pieceDataSets.offsets(),pieceDataSets.types() };
					parallelWrite(path, basename, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
					std::string filename = path + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu";

					auto written = readFile(filename);
					auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64/" + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu");

					CHECK(written == expected);
				}
				//check the .pvtu file
				auto written = readFile(path + basename + ".pvtu");
				auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64/" + basename + ".pvtu");

				CHECK(written == expected);
			}
			SECTION("test_pyramids3D_parallel_base64appended_3D")
			{
			    Base64BinaryAppendedWriter writer;
			    for (size_t fileId = 0; fileId < numberOfFiles; fileId++)
			    {
			    	Vtu11AllData pieceDataSets{ GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>(mesh, pointData, cellData, cellDistribution, fileId) };
			    	Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points(),pieceDataSets.connectivity(),pieceDataSets.offsets(),pieceDataSets.types() };
			    	parallelWrite(path, basename, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
			    	std::string filename = path + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu";
			    
			    	auto written = readFile(filename);
			    	auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64appended/" + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu");
			    
			    	CHECK(written == expected);
			    }
			    //check the .pvtu file
			    auto written = readFile(path + basename + ".pvtu");
			    auto expected = readFile("testfiles/parallel_write/pyramids_3D/base64appended/" + basename + ".pvtu");
			    
			    CHECK(written == expected);
			}
			SECTION("test_pyramids3D_parallel_raw_3D")
			{
				RawBinaryAppendedWriter writer;
				for (size_t fileId = 0; fileId < numberOfFiles; fileId++)
				{
					Vtu11AllData pieceDataSets{ GetCurrentDataSet<Vtu11UnstructuredMesh,Vtu11AllData>(mesh, pointData, cellData, cellDistribution, fileId) };
					Vtu11UnstructuredMesh pieceMesh{ pieceDataSets.points(),pieceDataSets.connectivity(),pieceDataSets.offsets(),pieceDataSets.types() };
					parallelWrite(path, basename, pieceMesh, pieceDataSets.pointData(), pieceDataSets.cellData(), fileId, numberOfFiles, writer);
					std::string filename = path + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu";

					auto written = readFile(filename);
					auto expected = readFile("testfiles/parallel_write/pyramids_3D/raw/" + basename + "/" + basename + "_" + std::to_string(fileId) + ".vtu");

					CHECK(written == expected);
				}
				//check the .pvtu file
				auto written = readFile(path + basename + ".pvtu");
				auto expected = readFile("testfiles/parallel_write/pyramids_3D/raw/" + basename + ".pvtu");

				CHECK(written == expected);
			}
			//Todo: 1.Add the creation of the subfolder to the parallel_write function (and check, if folder has been created)
			//		3.Finish the test!!!
			//		4.Write also appended and all others!
		}//TestCase
	}//namespace parallelHelper
}//namespace vtu11