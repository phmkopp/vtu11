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

#include <limits>

namespace vtu11
{
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

  if( writePvtuCalls )
  {
    writeEmptyTag( output, "PDataArray", attributes );
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
template<typename Writer>
void writePVTUfile(const std::string& path,
                   const std::string& baseName,
                   const std::vector<DataSet>& pointData,
                   const std::vector<DataSet>& cellData,
                   size_t numberOfFiles,
                   Writer writer)
{
  std::string parallelName = path + baseName + ".pvtu";
  std::ofstream output( parallelName, std::ios::binary );

  size_t ghostLevel = 0; //Hardcoded to be 0

  VTU11_CHECK( output.is_open( ), "Failed to open file \"" + baseName + "\"" );

  output << "<?xml version=\"1.0\"?>\n";

  StringStringMap headerAttributes{ { "byte_order",  endianness()       },
                                    { "type"      ,  "PUnstructuredGrid" },
                                    { "version"   ,  "0.1"              } };

  writer.addHeaderAttributes( headerAttributes );
  {
    ScopedXmlTag vtkFileTag( output, "VTKFile", headerAttributes );
    {
      ScopedXmlTag pUnstructuredGridFileTag(output, "PUnstructuredGrid", { { "GhostLevel", std::to_string(ghostLevel) } });

      {
        ScopedXmlTag pPointDataTag( output, "PPointData", { } );

        for( const auto& dataSet : pointData )
        {
          addDataSet(writer, output, std::get<2>(dataSet), std::get<1>(dataSet), std::get<0>(dataSet), true);
        }

      } // PPointData

      {
        ScopedXmlTag pCellDataTag( output, "PCellData", { } );

        for (const auto& dataSet : cellData)
        {
          addDataSet(writer, output, std::get<2>(dataSet), std::get<1>(dataSet), std::get<0>(dataSet), true);
        }

      } // PCellData

      {
        ScopedXmlTag pPointsTag( output, "PPoints", { } );

        StringStringMap attributes = { { "type", dataTypeString<double>() }, { "NumberOfComponents", std::to_string(3) } };

        writer.addDataAttributes( attributes );

        writeEmptyTag( output, "PDataArray", attributes );

      } // PPoints

      for( size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles )
      {
        std::string pieceName = baseName + "/" + baseName + "_" + std::to_string( nFiles ) + ".vtu";

        writeEmptyTag(output, "Piece", { { "Source", pieceName } } );

      } // Pieces
    } // PUnstructuredGrid
  } // PVTUFile

  output.close();

} // writePVTUfile

} // namespace detail


template<typename MeshGenerator, typename Writer>
void write( const std::string& filename,
            MeshGenerator& mesh,
            const std::vector<DataSet>& pointData,
            const std::vector<DataSet>& cellData,
            Writer writer)
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
                    const std::string& baseName,
                    MeshGenerator& mesh,
                    const std::vector<DataSet>& pointData,
                    const std::vector<DataSet>& cellData,
                    const size_t fileId,
                    const size_t numberOfFiles,
                    Writer writer)
{
  fs::path directory( path );

  // create base directory for all files if not existing
  if( !fs::exists( directory ) && fileId == 0 )
  {
    fs::create_directories( directory );
  }

  directory /= baseName;

  // create directory for vtu files if not existing
  if( !fs::exists( directory ) && fileId == 0 )
  {
    fs::create_directories( directory );
  }

  if( fileId == 0 )
  {
    detail::writePVTUfile( path, baseName, pointData, cellData, numberOfFiles, writer );
  }

  directory /= fs::path(baseName + "_" + std::to_string(fileId) + ".vtu");

  write( directory, mesh, pointData, cellData, writer );

} // parallelWrite
} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
