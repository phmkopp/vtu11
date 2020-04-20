//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#include "external/catch2/catch.hpp"
#include "vtu11.hpp"

#include <sstream>
#include <fstream>
#include <iostream> // remove

namespace vtu11
{
	TEST_CASE("hexahedras_test")
	{
		std::vector<double> points{
			0, 0, 0,	5, 0, 0,	0, 5, 0,	5, 5, 0, //0, 1, 2, 3
			0, 0, 5,	5, 0, 5,	0, 5, 5,	5, 5, 5, //4, 5, 6, 7
			2, 2, 5,	7, 2, 5,	2, 7, 5,	7, 7, 5, //8, 9, 10, 11
			2, 2, 10,	7, 2, 10,	2, 7, 10,	7, 7, 10 //12, 13, 14, 15
		};
		std::vector<VtkIndexType> connectivity{
			0, 1, 2, 3, 4, 5, 6, 7, //0
			8, 9, 10, 11, 12, 13, 14, 15 //1
			// hexahedra - cubes
		};
	
		std::vector<VtkCellType> types{ 11, 11 };
		std::vector<VtkIndexType> offsets{ 8, 16 };
		Vtu11UnstructuredMesh mesh{ points, connectivity, offsets, types };
			
		std::vector<double> pointData1{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5 };
		std::vector<double> pointData2{ 41, 13, 16, 81, 51, 31, 18, 12, 19, 21, 11, 19, 16, 45, 35, 58 };
		std::vector<double> cell_1{ 1, 2 };
		std::vector<double> cell_2{ 10, 20 };
		
		std::vector<DataSet> pointData
		{
			DataSet {std::string("Point_Data_1"), 1, pointData1},
			DataSet {std::string("Point_Data_2"), 1, pointData2}
		};

		std::vector<DataSet> cellData
		{
			DataSet {std::string("Cell_1"), 1, cell_1},
			DataSet {std::string("Cell_2"), 1, cell_2}
		};

		write("hexas_test.vtu", mesh, pointData, cellData);
	}
}
