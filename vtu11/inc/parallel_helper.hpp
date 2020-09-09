//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLEL_WRITE_HELPER_HPP
#define VTU11_PARALLEL_WRITE_HELPER_HPP

#include <array>
#include "vtu11.hpp"
#include "utilities.hpp"
namespace vtu11
{
	//writes the .pvtu file, that keeps together all vtu pieces
	template<typename Writer = AsciiWriter>
	void writePVTUfile(const std::string & path,
		const std::string & baseName,
		const std::vector<DataSet> & pointData,
		const std::vector<DataSet> & cellData,
		size_t fileId, size_t numberOfFiles,
		Writer writer = Writer( ) );

	namespace parallelHelper
	{
		//This function adjusts the numberOfFiles, if needed and returns:
		// 1.	The amount of cells per file-piece
		// 2.	The fileId of that file-piece, that has the highest id with the returned amount of cells. All pieces with a higher file-id have one cell less!!
		//		(only, if there are at least one cell for each requested piece. Otherwise the numberOfFiles gets adjusted!)
		inline std::array<size_t, 2> GetAmountOfCells(size_t * numberOfFiles, size_t numberOfCells);

		template<typename Writer, typename DataType>
		inline void addPEmptyDataSet(Writer& writer,
			std::ostream& output,
			const std::vector<DataType>& data,
			size_t numberOfComponents = 1,
			const std::string & name = "");
	}//namespace parallelHelper

}//namespace vtu11
#include "impl/parallel_helper_impl.hpp"
#endif //VTU11_PARALLEL_WRITE_HELPER_HPP