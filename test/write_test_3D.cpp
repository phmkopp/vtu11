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
	TEST_CASE("write_test_3D")
	{
		std::vector<double> points{
			0, 0, 0,    0, 3, 0,    1, 2, 2,  //0, 1, 2
			1, 3,-2,   -2, 2, 0,   -1, 1, 2,  //3, 4, 5
			2,-2,-2,    2,-2, 2,   -2,-2, 2,  //6, 7, 8
		   -2,-2,-2							  //9
		};
		std::vector<VtkIndexType> connectivity{
			5, 0, 1, 2,
			2, 0, 1, 3,
			3, 0, 1, 4,
			4, 0, 1, 5,
			8, 7, 6, 9, 0  //Pyramid
		};
		std::vector<VtkCellType> types{ 10, 10, 10, 10, 14 };
		std::vector<VtkIndexType> offsets{ 4, 8, 12, 16, 21 };
		Vtu11UnstructuredMesh mesh{ points, connectivity, offsets, types };

		std::vector<double> flashStrengthPoints{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
		std::vector<double> cellColour{ 1, 2, 3, 4, 0 };

		std::vector<DataSet> pointData{ DataSet{std::string("Flash Strength Points"), 1, flashStrengthPoints } };
		std::vector<DataSet> cellData{ DataSet{std::string("cell Colour"), 1, cellColour} };
		//write("ascii3D.vtu", mesh, pointData, cellData);

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
		std::string filename = "2x3_test.vtu";
		//std::string filename = "ascii.vtu";

		SECTION("ascii_3D")
		{
			REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData));

			auto written = readFile(filename);
			auto expected = readFile("testfiles/3D_Test/ascii.vtu");

			CHECK(written == expected);
		}

		// The files assume that, we need to add a big endian version
		REQUIRE(endianness() == "LittleEndian");
		//filename = "base64.vtu";
		SECTION("base64_3D")
		{
			Base64BinaryWriter writer;

			REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData, writer));

			auto written = readFile(filename);
			auto expected = readFile("testfiles/3D_Test/base64.vtu");

			CHECK(written == expected);
		}
		//The file base64appended.vtu still cannot be opened within ParaView!!!
		filename = "base64appended.vtu";
		SECTION("base64appended_3D")
		{
			Base64BinaryAppendedWriter writer;

			REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData, writer));

			auto written = readFile(filename);
			auto expected = readFile("testfiles/3D_Test/base64appended.vtu");

			CHECK(written == expected);
		}
		filename = "raw.vtu";
		SECTION("raw_3D")
		{
			RawBinaryAppendedWriter writer;

			REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData, writer));

			auto written = readFile(filename);
			auto expected = readFile("testfiles/3D_Test/raw.vtu");

			CHECK(written == expected);
		}
		//filename = "raw_compressed.vtu";
#ifdef VTU11_ENABLE_ZLIB
		SECTION("raw_compressed")
		{
			CompressedRawBinaryAppendedWriter writer;

			REQUIRE_NOTHROW(write(filename, mesh, pointData, cellData, writer));

			auto written = readFile(filename);
			auto expected = readFile("testfiles/2x3_compressed.vtu");

			CHECK(written == expected);
		}
#endif

	}
}
