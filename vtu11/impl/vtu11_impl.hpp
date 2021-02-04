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

#include "external/filesystem/filesystem.hpp"
#include "inc/xml.hpp"
#include "inc/utilities.hpp"
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
                        const std::string& name = "" ,
	                    bool writePvtuCalls = false)
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
  if (writePvtuCalls)
  {
	  writeEmptyTag(output, "PDataArray", attributes);
  }
  else if( attributes["format"] != "appended" )
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
//Each piece consists of a set of points and cells
template<typename MeshGenerator, typename Writer>
void parallelWrite( const std::string& path,
                    std::string baseName,
                    MeshGenerator& mesh,
                    const std::vector<DataSet>& pointData,
                    const std::vector<DataSet>& cellData,
                    size_t fileId, size_t numberOfFiles,
                    Writer writer )
{
	//ToDo: Take care of cleaning the folder! not done in this code as Kratos takes care of it
    fs::path p1 = path;
    p1.make_preferred();
    if( !fs::exists( p1 ) )
    {
      fs::create_directory( p1 );
      std::cout << "Original path, where the parallel files should be stored, does not exist!" << std::endl;
    }

    fs::path directory = path + baseName + "/";
    directory.make_preferred();
    if( !fs::exists( directory ) )
    {
      fs::create_directory( directory );
    }

  if( fileId == 0 )
  {
    vtu11::writePVTUfile( path, baseName, pointData, cellData, numberOfFiles, writer );
  }
  fs::path name = path + baseName + "/" + baseName + "_" + std::to_string(fileId) + ".vtu";
  name.make_preferred();

  write( name, mesh, pointData, cellData, writer );

} // parallelWrite
} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
