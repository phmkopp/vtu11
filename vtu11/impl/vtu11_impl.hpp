//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_VTU11_IMPL_HPP
#define VTU11_VTU11_IMPL_HPP

#include "inc/xml.hpp"
#include "inc/utilities.hpp"
#include "inc/parallel_helper.hpp"
#include <limits>

namespace vtu11
{

/*
 * todo
 * - check consistency of each point & cell data set
 */

namespace detail
{

template<typename Writer, typename DataType>
inline void addDataSet( Writer& writer,
                        std::ostream& output,
                        const std::vector<DataType>& data,
                        size_t numberOfComponents = 1,
                        const std::string& name = "" )
{
  StringStringMap attributes = { { "type", dataTypeString<DataType>( ) } };

  if( numberOfComponents > 1 )
  {
    attributes["NumberOfComponents"] = std::to_string( numberOfComponents );
  }

  if( name != "" )
  {
    attributes["Name"] = name;
  }

  writer.addDataAttributes( attributes );

  if( attributes["format"] != "appended" )
  {
    ScopedXmlTag dataArrayTag( output, "DataArray", attributes );

    writer.writeData( output, data );
  }
  else
  {
    writeEmptyTag( output, "DataArray", attributes );

    writer.writeData( output, data );
  }
}

/* ToDo: write this function in the utilities header
 *       or create a proper header for this parallel_helper.hpp
 * NOTES: Could we somehow add this to the original addDataSet via another input argument,
 *        e.g. "Bool = true/false" for Parallel, and then add an if-else statement to use
 *        the writeEmptyTag function accordingly?
 */
template<typename Writer, typename DataType>
inline void addPEmptyDataSet( Writer& writer,
                             std::ostream& output,
                             const std::vector<DataType>& data,
                             size_t numberOfComponents = 1,
                             const std::string& name = "" )
{
  StringStringMap attributes = { { "type", dataTypeString<DataType>( ) } };

  if( numberOfComponents > 1 )
  {
    attributes["NumberOfComponents"] = std::to_string( numberOfComponents );
  }

  if( name != "" )
  {
    attributes["Name"] = name;
  }

  writer.addDataAttributes( attributes );

  writeEmptyTag( output, "PDataArray", attributes );
}

} // namespace detail


template<typename MeshGenerator, typename Writer>
void write( const std::string& filename,
            MeshGenerator& mesh,
            const std::vector<DataSet>& pointData,
            const std::vector<DataSet>& cellData,
            Writer writer )
{
	std::ofstream output(filename, std::ios::binary);

  VTU11_CHECK( output.is_open( ), "Failed to open file \"" + filename + "\"" );

  output << "<?xml version=\"1.0\"?>\n";

  StringStringMap headerAttributes { { "byte_order",  endianness( )      },
                                     { "type"      ,  "UnstructuredGrid" },
                                     { "version"   ,  "0.1"              } };

  writer.addHeaderAttributes( headerAttributes );

  {
    ScopedXmlTag vtkFileTag( output, "VTKFile", headerAttributes );
    {
      ScopedXmlTag unstructuredGridFileTag( output, "UnstructuredGrid", { } );
      {
        ScopedXmlTag pieceTag( output, "Piece", { { "NumberOfPoints", std::to_string( mesh.numberOfPoints( ) ) },
                                                  { "NumberOfCells" , std::to_string( mesh.numberOfCells( ) )    } } );

        {
          ScopedXmlTag pointDataTag( output, "PointData", { } );

          for( const auto& dataSet : pointData )
          {
            detail::addDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }

        } // PointData

        {
          ScopedXmlTag cellDataTag( output, "CellData", { } );

          for( const auto& dataSet : cellData )
          {
            detail::addDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }

        } // CellData

        {
          ScopedXmlTag pointsTag( output, "Points", { } );

          detail::addDataSet( writer, output, mesh.points( ), 3 );

        } // Points

        {
          ScopedXmlTag pointsTag( output, "Cells", { } );

          detail::addDataSet( writer, output, mesh.connectivity( ), 1, "connectivity" );
          detail::addDataSet( writer, output, mesh.offsets( ), 1, "offsets" );
          detail::addDataSet( writer, output, mesh.types( ), 1, "types" );

        } // Cells

      } // Piece
    } // UnstructuredGrid

    auto appendedAttributes = writer.appendedAttributes( );

    if( !appendedAttributes.empty( ) )
    {
      ScopedXmlTag appendedDataTag( output, "AppendedData", appendedAttributes );

      output << "_";

      writer.writeAppended( output );

    } // AppendedData
  } // VTKFile

  output.close( );
}

//ParallelWrite generates a pvtu file and accordingly the vtu pieces in a subfolder
//Each piece consists of a set of points and using those points some full cells
template<typename MeshGenerator, typename Writer, typename AllMeshData>
void parallelWrite( const std::string& path,
                    const std::string& baseName,
                    MeshGenerator& mesh,
                    const std::vector<DataSet>& pointData,
                    const std::vector<DataSet>& cellData,
                    size_t fileId, size_t numberOfFiles,
                    Writer writer )
{
	//ToDo: Write a function that distributes the data into "numberOfFiles" equal pieces!!
	//This function finds the amount of cells per piece
	std::array<size_t, 2> distributer = vtu11::parallelHelper::GetAmountOfCells(&numberOfFiles, mesh.numberOfCells());
	//ToDo: Start the parallelSection here! 
    //Here OpenMp or MPI is needed to distribute all data and write the files in parallel!!!
    //The commands are from MPI, right now, but OpenMP is probably easier!
    //Probably dataParallelism (distribute the data) is best here to make it work in parallel!

    //OpenMP: Use a parallel for loop as dataParallelism will probably be the only useful thing here
    //        Make a dependency Analysis and think about the status of all variables!! (private, lastprivate,.....)

    //MPI: The Master process Scatters the data to distribute it to all other processes --> a Send message is needed and all other processes needs to receive it
    //        Think about the way of sending!!!! (synchronous, buffered,...)

//start the parallel part with this command
//Until MPI_finalize, the processes work in parallel
//ToDo: MPI_Init(int*argc,char***argv)

//Get the actual rank of the process, that is working in exactly this parallel part
//useful for determining the fileId of the actual process
//ToDo: MPI_Comm_rank(MPI_COMM_WORLD, int &rank)

    if (fileId == 0)//Here fileId=rank
    {
        //Finding the amount of processes, that are working in the Communicator = numberOfFiles
        //ToDo: MPI_Comm_size(MPI_COMM_WORLD, int &size)

        //If MPI is used: The master process distributes the data into smaller, equally sized pieces and sends it to every other process
        //Think about the way of sending (asynchronous, buffered,...)
        //Is a Wait command necessary somewhere?
        //ToDo: MPI_Scatter.......
        //This function writes the one .pvtu file, that keeps together all pieces, that contain the actual data
      // Eulogio comment out: writePVTUfile(path, baseName, fileId, numberOfFiles);//why fileId here? fileId of pvtu file is always the master process, so probably 0
      
      // ToDo: Leave it in utilities or in parallel_helper?
      vtu11::writePVTUfile( path, baseName, pointData, cellData, fileId, numberOfFiles, writer );

    }
    //else { --> I think an else is needed here, as the master process is not doing, what all other processes are doing



        
    //OpenMP: The status of pointData, cellData and mesh is very important!!!
    //        Do they need to be private, lastprivate, public, ... ? --> probably public, as only parts of the data are taken!!!
    //        The for loop is doing the data distribution, but it needs to be an openMP parallel loop!!
    //            
    //If OpenMp is used, a parallel for loop is probably necessary, will look something like that
    //ToDo: #pragma omp for [what about the status of all variables?, clauses,...]e.g.: for (i=1;i<=numberOfFiles;i++){


	AllMeshData pieceData =
		parallelHelper::GetCurrentDataSet <MeshGenerator,AllMeshData> (mesh, pointData, cellData, distributer, fileId /*=rank*/);
    //rank is the same as the fileId!!
    std::string name = path + baseName + "/" + baseName + "_" + std::to_string(fileId) + ".vtu";

    
    // write(name, mesh(somehow dependent of i), pointData(i), cellData(i), writer);
    write(name, pieceData.mesh(), pieceData.pointData(), pieceData.cellData(), writer);
    //} until here goes the for loop of OpenMP
    //} the final bracket of the else loop
    
//MPI_Finalize(void)
}

} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
