//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLELWRITEHELPER_HPP
#define VTU11_PARALLELWRITEHELPER_HPP

#include <array>

namespace vtu11
{

	namespace parallelHelper
	{
		//This function adjusts the numberOfFiles, if needed and returns:
		// 1.	The amount of cells per file-piece
		// 2.	The fileId of that file-piece, that has the highest id with the returned amount of cells. All pieces with a higher file-id have one cell less!!
		//		(only, if there are at least one cell for each requested piece. Otherwise the numberOfFiles gets adjusted!)
		inline std::array<size_t, 2> GetAmountOfCells(size_t * numberOfFiles, size_t numberOfCells);
	}//namespace parallelHelper

}//namespace vtu11
#include "impl/parallelWriteHelper_impl.hpp"
#endif //VTU11_PARALLELWRITEHELPER_HPP


