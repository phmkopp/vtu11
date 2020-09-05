//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLELWRITEHELPER_IMPL_HPP
#define VTU11_PARALLELWRITEHELPER_IMPL_HPP
#include <iostream>

namespace vtu11
{
	namespace parallelHelper
	{
		std::array<size_t, 2> GetAmountOfCells(size_t * numberOfFiles, size_t numberOfCells)
		{
			size_t cellsPerFile = 1;
			//Check if the number of cells can be distributed equally to each file-piece
			if (numberOfCells % *numberOfFiles == 0)
			{
				cellsPerFile = numberOfCells / *numberOfFiles;
				return { cellsPerFile , *numberOfFiles };
			}
			//If there are less cells then numberOfFiles, the numberOfFiles-Variable needs to be adjusted,
			//so that each file-piece contains all data, that concerns exactly one cell
			else if (floor(numberOfCells / *numberOfFiles) == 0.)
			{
				*numberOfFiles = numberOfCells;
				return { cellsPerFile,*numberOfFiles };
			}
			//If the numberOfCells is not divisible by the number of Files, there are "cellsPerFile" cells in the first file-pieces
			//and "cellsPerFile - 1" cells in all pieces, with the fileId > lastFileWithCellsPerFile
			else if (floor(numberOfCells / *numberOfFiles) > 0.)
			{
				cellsPerFile = ceil(numberOfCells / *numberOfFiles);
				size_t lastFileWithCellsPerFile = numberOfCells % *numberOfFiles;
				std::cout << "The number of files has been decreased to " << *numberOfFiles << ", as there are not that many cells!" << std::endl;
				return { cellsPerFile, lastFileWithCellsPerFile };
			}
			else
			{
				throw std::exception("Determining the amount of cells per file-piece went wrong!");
			}

		}
	}
}
#endif //VTU11_PARALLELWRITEHELPER_IMPL_HPP

