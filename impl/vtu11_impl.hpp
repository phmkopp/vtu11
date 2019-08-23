#ifndef VTU11_VTU11_IMPL_HPP
#define VTU11_VTU11_IMPL_HPP

#include "xml.hpp"
#include "utilities.hpp"

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
inline void addDataSet( std::ostream& output,
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
  
  Writer::appendDataAttributes( attributes );

  ScopedXmlTag dataArrayTag( output, "DataArray", attributes );
           
  Writer::writeData( output, data );
  
  output << "\n";
}

} // namespace detail

template<typename Writer>
inline void write( std::ostream& output, 
                   const UnstructuredMesh& mesh,
                   const std::vector<DataSet>& pointData,
                   const std::vector<DataSet>& cellData  )
{

  output << "<?xml version=\"1.0\"?>\n";
  
  StringStringMap headerAttributes { { "byte_order",  "LittleEndian"     },
                                     { "type"      ,  "UnstructuredGrid" },
                                     { "version"   ,  "0.1"              } };
                                     
  Writer::appendHeaderAttributes( headerAttributes );
  
  { 
    ScopedXmlTag vtkFileTag( output, "VTKFile", headerAttributes );
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
            detail::addDataSet<Writer>( output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }
          
        } // PointData
        
        {
          ScopedXmlTag cellDataTag( output, "CellData", { } );
          
          for( const auto& dataSet : cellData )
          {
            detail::addDataSet<Writer>( output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }
          
        } // CellData
        
        {
          ScopedXmlTag pointsTag( output, "Points", { } );

          detail::addDataSet<Writer>( output, mesh.points, 3 );
          
        } // Points
        
        {
          ScopedXmlTag pointsTag( output, "Cells", { } );

          detail::addDataSet<Writer>( output, mesh.connectivity, 1, "connectivity" );
          detail::addDataSet<Writer>( output, mesh.offsets, 1, "offsets" );
          detail::addDataSet<Writer>( output, mesh.types, 1, "types" );
          
        } // Cells
        
      } // Piece
    } // UnstructuredGrid
  } // VTKFile
}

} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
