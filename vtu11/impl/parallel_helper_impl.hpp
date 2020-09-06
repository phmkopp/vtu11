//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLEL_WRITE_HELPER_IMPL_HPP
#define VTU11_PARALLEL_WRITE_HELPER_IMPL_HPP
#include <iostream>
#include "inc/xml.hpp"
#include "vtu11_impl.hpp"
namespace vtu11
{
	// Declaration of writePVTUfile(path, baseName, fileId, numberOfFiles);
	//numberOfFiles returns the number of all created vtu pieces
	//fileId returns the actual ghost level?
	template<typename Writer>
	void writePVTUfile(const std::string& path,
		const std::string& baseName,
		const std::vector<DataSet>& pointData,
		const std::vector<DataSet>& cellData,
		size_t fileId, size_t numberOfFiles,
		Writer writer)
	{
		std::string parallelName = path + baseName + ".pvtu";
		std::ofstream output(parallelName, std::ios::binary);
		size_t ghostLevel = 0;
		VTU11_CHECK(output.is_open(), "Failed to open file \"" + baseName + "\"");

		output << "<?xml version=\"1.0\"?>\n";
		//Std::Map is a class, that contains key-value pairs with unique keys
			//Header Attributes of the PvtuFile
		StringStringMap headerAttributes{ { "byte_order",  endianness()       },
										  { "type"      ,  "PUnstructuredGrid" },
										  { "version"   ,  "0.1"              } };

		writer.addHeaderAttributes(headerAttributes);

		//ScopedXmlTag is a class
		{
			ScopedXmlTag vtkFileTag(output, "VTKFile", headerAttributes);
			{
				//Does the file Id really define the ghost level?
				//I think we should start pretending, that we have always ghostLevel=0, as it would make everything much harder in the beginning.
				// we can still change it in the end, if everything else works... What do you think?
				ScopedXmlTag pUnstructuredGridFileTag(output, "PUnstructuredGrid", { { "GhostLevel", std::to_string(fileId) } });
				{
					//What Point Data comes here?
					ScopedXmlTag pPointDataTag(output, "PPointData", { });

					for (const auto& dataSet : pointData)
					{
						parallelHelper::addPEmptyDataSet(writer, output, std::get<2>(dataSet), std::get<1>(dataSet), std::get<0>(dataSet));
					}
				} // PPointData
				{
					ScopedXmlTag pCellDataTag(output, "PCellData", { });

					for (const auto& dataSet : cellData)
					{
						parallelHelper::addPEmptyDataSet(writer, output, std::get<2>(dataSet), std::get<1>(dataSet), std::get<0>(dataSet));
					}
				} // PCellData

				// ToDo: -absolute path or relative path? Currently relative path, results appear in build folder.
				//       -should we store this somehow? And reuse for the processes to write. I think not, we can create the name on the fly
				//       -Janina: I think relative path should be all right in the beginning, we can still change it later though
				for (size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles)
				{
					std::string pieceName = path + baseName + "/" + baseName + "_" + std::to_string(nFiles) + ".vtu";
					writeEmptyTag(output, "Piece", { { "Source", pieceName } });
				} // Pieces

			} // PUnstructuredGrid
		} // VTKFile

		output.close();
	}
	namespace parallelHelper
	{
		/* ToDo: write this function in the utilities header
 *       or create a proper header for this parallel_helper.hpp
 * NOTES: Could we somehow add this to the original addDataSet via another input argument,
 *        e.g. "Bool = true/false" for Parallel, and then add an if-else statement to use
 *        the writeEmptyTag function accordingly?
 */
		template<typename Writer, typename DataType>
		inline void addPEmptyDataSet(Writer& writer,
			std::ostream& output,
			const std::vector<DataType>& data,
			size_t numberOfComponents,
			const std::string & name)
		{
			StringStringMap attributes = { { "type", dataTypeString<DataType>() } };

			if (numberOfComponents > 1)
			{
				attributes["NumberOfComponents"] = std::to_string(numberOfComponents);
			}

			if (name != "")
			{
				attributes["Name"] = name;
			}

			writer.addDataAttributes(attributes);

			writeEmptyTag(output, "PDataArray", attributes);
		}//addPEmptyDataSet
//________________________________________________________________________________________________________________________________
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
#endif //VTU11_PARALLEL_WRITE_HELPER_IMPL_HPP

