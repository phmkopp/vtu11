#ifndef VTU11_VTU11_IMPL_HPP
#define VTU11_VTU11_IMPL_HPP

#include "xml.hpp"
#include "filesystem.hpp"

#include <limits>

namespace vtu11
{

/* 
 * todo
 * - check consistency of each point & cell data set  
 */
 
namespace detail
{

class AsciiWriter
{
  void writeData(  );

};


template<typename DataType>
std::string dataTypeString( )
{
  std::string base;
  
  if( std::numeric_limits<DataType>::is_integer and std::numeric_limits<DataType>::is_signed )
  {
      base = "Int";
  }
  else if( std::numeric_limits<DataType>::is_integer and not std::numeric_limits<DataType>::is_signed )
  {
      base = "UInt";
  }
  else
  {
    base = "Float";
  }
  
  return base + std::to_string( sizeof( DataType ) * 8 );
}

template<typename DataType>
inline void writeDataSet( std::ostream& output,
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

  ScopedXmlTag dataArrayTag( output, "DataArray", attributes );
           
  for( auto value : data )
  {
      output << value << " ";
  }
  
  output << "\n";
}

} // namespace detail


inline void write( std::ostream& output, 
                   const UnstructuredMesh& mesh,
                   const std::vector<DataSet>& pointData,
                   const std::vector<DataSet>& cellData  )
{

  output << "<?xml version=\"1.0\"?>\n";
  
  { 
    ScopedXmlTag vtkFileTag( output, "VTKFile", { { "byte_order",  "LittleEndian"     },
                                                  { "type"      ,  "UnstructuredGrid" },
                                                  { "version"   ,  "0.1"              } } );
    { 
      ScopedXmlTag unstructuredGridFileTag( output, "UnstructuredGrid", { } );
      { 
        size_t numberOfPoints = mesh.points.size( ) / 3;
      
        ScopedXmlTag pieceTag( output, "Piece", { { "NumberOfPoints", std::to_string( numberOfPoints ) },
                                                  { "NumberOfCells" , std::to_string( mesh.offsets.size( ) )    } } );
        
        {
          ScopedXmlTag pointDataTag( output, "PointData", { } );
          
          for( const auto& dataSet : pointData )
          {
            detail::writeDataSet( output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }
          
        } // PointData
        
        {
          ScopedXmlTag cellDataTag( output, "CellData", { } );
          
          for( const auto& dataSet : cellData )
          {
            detail::writeDataSet( output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }
          
        } // CellData
        
        {
          ScopedXmlTag pointsTag( output, "Points", { } );

          detail::writeDataSet( output, mesh.points, 3 );
          
        } // Points
        
        {
          ScopedXmlTag pointsTag( output, "Cells", { } );

          detail::writeDataSet( output, mesh.connectivity, 1, "connectivity" );
          detail::writeDataSet( output, mesh.offsets, 1, "offsets" );
          detail::writeDataSet( output, mesh.types, 1, "types" );
          
        } // Cells
        
      } // Piece
    } // UnstructuredGrid
  } // VTKFile
}

} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
