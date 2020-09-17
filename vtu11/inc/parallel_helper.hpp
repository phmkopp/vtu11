//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLEL_HELPER_HPP
#define VTU11_PARALLEL_HELPER_HPP

#include <array>
#include "alias.hpp"
#include "vtu11.hpp"
#include "utilities.hpp"
namespace vtu11
{
	struct Vtu11AllData
	{
		std::vector<double> points_;
		std::vector<VtkIndexType> connectivity_;
		std::vector<VtkIndexType> offsets_;
		std::vector<VtkCellType> types_;
		std::vector<DataSet> pointData_;
		std::vector<DataSet> cellData_;
		
		std::vector<double>& points() { return points_; }
		std::vector<VtkIndexType>& connectivity() { return connectivity_; }
		std::vector<VtkIndexType>& offsets() { return offsets_; }
		std::vector<VtkCellType>& types() { return types_; }
		std::vector<DataSet>& pointData() { return pointData_; }
		std::vector<DataSet>& cellData() { return cellData_; }
		size_t numberOfPoints() { return points_.size() / 3; }
		size_t numberOfCells() { return types_.size(); }
	};
	//writes the .pvtu file, that keeps together all vtu pieces
	template<typename Writer = AsciiWriter>
	void writePVTUfile(const std::string & path,
		const std::string & baseName,
		const std::vector<DataSet> & pointData,
		const std::vector<DataSet> & cellData,
		size_t fileId, size_t numberOfFiles,
		Writer writer = Writer());

	namespace parallelHelper
	{
		//This function adjusts the numberOfFiles, if needed and returns:
		// 1.	The amount of cells per file-piece
		// 2.	The fileId of that file-piece, that has the highest id with the returned amount of cells. All pieces with a higher file-id have one cell less!!
		//		(only, if there are at least one cell for each requested piece. Otherwise the numberOfFiles gets adjusted!)
		inline std::array<size_t, 2> GetAmountOfCells(size_t * numberOfFiles, size_t numberOfCells);

		//This function distributes the global mesh into equal small pieces and returns those mesh-pieces and the associated data
		//There is no fileId 0, it starts with fileId==1
		template<typename MeshGenerator, typename AllMeshData>
		AllMeshData GetCurrentDataSet(MeshGenerator& mesh,
				const std::vector<DataSet>& pointData,
				const std::vector<DataSet>& cellData,
				std::array<size_t, 2> cellDistribution,
				size_t fileId);

		//This function returns the cell and point datasets only for one specific piece
		inline std::array<std::vector<DataSet>, 2> GetCurrentCellPointData(const std::vector<DataSet> & pointDataGlobal,
			const std::vector<DataSet> & cellDataGlobal,
			std::vector<VtkIndexType> & globalTranslation,
			size_t firstCellId, size_t lastCellId);

		//This function adds an empty Parallel Dataset to the xml-format
		template<typename Writer, typename DataType>
		inline void addPEmptyDataSet(Writer& writer,
			std::ostream& output,
			const std::vector<DataType>& data,
			size_t numberOfComponents = 1,
			const std::string & name = "");
	}//namespace parallelHelper

}//namespace vtu11
#include "impl/parallel_helper_impl.hpp"
#endif //VTU11_PARALLEL_HELPER_HPP