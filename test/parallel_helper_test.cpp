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
				result = { 2,1 };
				CHECK(GetAmountOfCells(&numberOfFiles, numberOfCells) == result);
			}
			SECTION("checkThrow")
			{
				numberOfFiles = 0;
				numberOfCells = 6;
				REQUIRE_THROWS(GetAmountOfCells(&numberOfFiles, numberOfCells));
			}
		}
		
	}//namespace parallelHelper
}//namespace vtu11

