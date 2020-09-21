//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLEL_HELPER_IMPL_HPP
#define VTU11_PARALLEL_HELPER_IMPL_HPP

#include <iostream>
#include <math.h>
#include "inc/xml.hpp"


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
		std::vector<double> points;
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
				{
					ScopedXmlTag pPointsTag(output, "PPoints", {});
					parallelHelper::addPEmptyDataSet(writer, output, points, 3, "");
					//add:
					//<PPoints>
					//<DataArray NumberOfComponents = "3" format = "ascii" type = "Float64" / >
					//< / PPoints>
				} // PPoints
				// ToDo: -absolute path or relative path? Currently relative path, results appear in build folder.
				//       -should we store this somehow? And reuse for the processes to write. I think not, we can create the name on the fly
				//       -Janina: I think relative path should be all right in the beginning, we can still change it later though
				for (size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles)
				{
					std::string pieceName = baseName + "/" + baseName + "_" + std::to_string(nFiles) + ".vtu";
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
 *        -->Would be probably the best, if we can somehow include it.
 *        The declaration of this is included in the parallel_helper header, for now.
 */
		template<typename Writer, typename DataType>
		inline void addPEmptyDataSet(Writer& writer,
			std::ostream& output,
			const std::vector<DataType>& data,
			size_t numberOfComponents,
			const std::string& name)
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
			if (*numberOfFiles == 0)
			{
				throw std::exception("Determining the amount of cells per file-piece went wrong!");
			}
			else if (numberOfCells % *numberOfFiles == 0)
			{
				cellsPerFile = numberOfCells / *numberOfFiles;
				return { cellsPerFile , *numberOfFiles };
			}
			//If there are less cells then numberOfFiles, the numberOfFiles-Variable needs to be adjusted,
			//so that each file-piece contains all data, that concerns exactly one cell
			else if (floor(numberOfCells / double(*numberOfFiles)) == 0.)
			{
				*numberOfFiles = numberOfCells;
				std::cout << "The number of files has been decreased to " << *numberOfFiles << ", as there are not that many cells!" << std::endl;
				return { cellsPerFile,*numberOfFiles };
			}
			//If the numberOfCells is not divisible by the number of Files, there are "cellsPerFile" cells in the first file-pieces
			//and "cellsPerFile - 1" cells in all pieces, with the fileId > lastFileWithCellsPerFile
			else if (floor(numberOfCells / *numberOfFiles) > 0.)
			{
				cellsPerFile = ceil(numberOfCells / double(*numberOfFiles));
				size_t lastFileWithCellsPerFile = numberOfCells % *numberOfFiles-1;
				return { cellsPerFile, lastFileWithCellsPerFile };
			}
			else
			{
				throw std::exception("Determining the amount of cells per file-piece went wrong!");
			}

		}

		template<typename MeshGenerator, typename AllMeshData>
		inline AllMeshData GetCurrentDataSet(MeshGenerator& mesh,
				const std::vector<DataSet>& pointData,
				const std::vector<DataSet>& cellData,
				std::array<size_t, 2> cellDistribution,
				size_t fileId)
		{
			std::vector<double> points;
			std::vector<VtkIndexType> relevantConnectivity, connectivity, offsets;
			std::vector<VtkCellType> types;
			size_t firstCellId, lastCellId;
			if (fileId <= cellDistribution[1])
			{
				//There is no change in the amount of cells per file
				firstCellId = fileId * cellDistribution[0]; //Id of the first cell in that piece
				lastCellId = fileId * cellDistribution[0]+cellDistribution[0]; //Id of the last cell + 1 in that piece
			}
			else
			{
				//There is a change in the amount of cells per file
				firstCellId = (cellDistribution[1]+1) * cellDistribution[0] + (fileId - cellDistribution[1]-1)
					* (cellDistribution[0] - 1);//Id of the first cell in that piece
				lastCellId = (cellDistribution[1]+1) * cellDistribution[0] + (fileId - cellDistribution[1])
					* (cellDistribution[0] - 1);//Id of the last cell + 1 in that piece
			}
			//loop over all cells in that piece to store the new types, new offsets and select the relevant connectivity
			for (size_t currentCell = firstCellId; currentCell < lastCellId; ++currentCell)
			{
				if (firstCellId == 0)
				{
					offsets.push_back(mesh.offsets()[currentCell]);
				}
				else
				{
					offsets.push_back(mesh.offsets()[currentCell] - mesh.offsets()[firstCellId - 1]);
				}
				types.push_back(mesh.types()[currentCell]);

				//Determine the number of Points a cell consists of
				size_t numberOfCellPoints;
				if (currentCell == 0) { numberOfCellPoints = mesh.offsets()[currentCell]; }
				else
				{
					numberOfCellPoints = mesh.offsets()[currentCell] - mesh.offsets()[currentCell - 1];
				}

				//loop over each point of each cell in that piece to get the connectivity
				for (size_t cellPoint = numberOfCellPoints; cellPoint > 0; --cellPoint)
				{
					size_t actualPointIndex = mesh.connectivity()[mesh.offsets()[currentCell] - cellPoint];
					relevantConnectivity.push_back(actualPointIndex);
				}
			}//loop over all cells

			//Todo: put this section in an external function? 
			//How to find the points, that are needed in that piece?
			//1. loop over connectivity
			//2. check if the link to this connectivity-point already exists
			// -->if yes, just add the local connectivity to the connectivity piece
			// -->if no,
			//			I. add point to global translation vector, to know, what the original index of this point was
			//			II. add new point coordinates to the local vector
			//			III. add new point index (counter) to the connectivity vector 

			VtkIndexType counter = 0;
			std::vector<VtkIndexType> globalTranslation;
			for (VtkIndexType connection : relevantConnectivity)
			{
				auto piecePointId = std::find(globalTranslation.begin(), globalTranslation.end(), connection);
				if (piecePointId == globalTranslation.end())//If it is a new point, add new connectivity, add the global connectivity to the globalTranslation and add the new point to the local points
				{
					connectivity.push_back(counter);
					globalTranslation.push_back(connection);
					//loop over x-, y- and z-coordinate points and store all points in the piece order in piece internal points
					for (size_t coordinate = 0; coordinate < 3; ++coordinate)
					{
						points.push_back(mesh.points()[connection * 3 + coordinate]);
					}
					counter++;
				}
				else//if the point already exists, just add the new connectivity
				{
					VtkIndexType newConnectivity = std::distance(globalTranslation.begin(), piecePointId);
					connectivity.push_back(newConnectivity);
				}
			}
			std::array < std::vector<DataSet>, 2> pointCellData = GetCurrentCellPointData(pointData, cellData, globalTranslation, firstCellId, lastCellId);
			AllMeshData allDataPiece{ points, connectivity, offsets, types, pointCellData[0], pointCellData[1] };
			return allDataPiece; 
		}//GetCurrentDataSet

		inline std::array<std::vector<DataSet>, 2> GetCurrentCellPointData(const std::vector<DataSet> & pointDataGlobal,
			const std::vector<DataSet> & cellDataGlobal,
			std::vector<VtkIndexType> & globalTranslation,
			size_t firstCellId, size_t lastCellId)
		{
			std::vector<DataSet> pointData, cellData;
			for (DataSet pointSet : pointDataGlobal)
			{
				std::vector<double> data;
				for (VtkIndexType pointId : globalTranslation)
				{
					data.push_back(std::get<2>(pointSet)[pointId]);
				}
				pointData.push_back(DataSet{ std::get<0>(pointSet),std::get<1>(pointSet),data });
			}
			for (DataSet cellSet : cellDataGlobal)
			{
				std::vector<double> data;
				for (size_t cellId = firstCellId; cellId < lastCellId; ++cellId)
				{
					data.push_back(std::get<2>(cellSet)[cellId]);
				}
				cellData.push_back(DataSet{ std::get<0>(cellSet),std::get<1>(cellSet),data });
			}
			return { pointData, cellData };
		}//GetCurrentCellPointData
	}//namespace parallelHelper
}//namespace vtu11
#endif //VTU11_PARALLEL_HELPER_IMPL_HPP

