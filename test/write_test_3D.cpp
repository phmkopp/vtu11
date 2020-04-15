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
	TEST_CASE("writeAscii_test_3D")
	{
		std::vector<double> points{
			0, 0, 0,    0, 3, 0,    1, 2, 2,  //0, 1, 2
			1, 3,-2,   -2, 2, 0,   -1, 1, 2,  //3, 4, 5
			2,-2,-2,    2,-2, 2,   -2,-2, 2,  //6, 7, 8
		   -2,-2,-2							  //9
		};
		std::vector<_int64> connectivity{
			5, 0, 1, 2,
			2, 0, 1, 3,
			3, 0, 1, 4,
			4, 0, 1, 5,
			8, 7, 6, 9, 0  //Pyramid
		};
		std::vector<VtkCellType> types{ 10, 10, 10, 10, 14 };
		std::vector<_int64> offsets{ 4, 8, 12, 16, 21 };
		Vtu11UnstructuredMesh mesh{ points, connectivity, offsets, types };

		std::vector<double> flashStrengthPoints{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
		std::vector<double> cellColour{ 1, 2, 3, 4, 0 };

		std::vector<DataSet> pointData{ DataSet{std::string("Flash Strength Points"), 1, flashStrengthPoints } };
		std::vector<DataSet> cellData{ DataSet{std::string("cell Colour"), 1, cellColour} };
	}
}