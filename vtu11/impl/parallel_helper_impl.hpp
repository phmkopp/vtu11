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
  template<typename Writer>
  void writePVTUfile( const std::string& path,
                      std::string baseName,
                      const std::vector<DataSet>& pointData,
                      const std::vector<DataSet>& cellData,
                      size_t numberOfFiles,
                      Writer writer )
  {
    std::string parallelName = path + baseName + ".pvtu";
    std::ofstream output( parallelName, std::ios::binary );    
    size_t ghostLevel = 0;
    std::vector<double> points;
    VTU11_CHECK( output.is_open( ), "Failed to open file \"" + baseName + "\"" );

    output << "<?xml version=\"1.0\"?>\n";
    StringStringMap headerAttributes{ { "byte_order",  endianness()       },
                                      { "type"      ,  "PUnstructuredGrid" },
                                      { "version"   ,  "0.1"              } };

    writer.addHeaderAttributes( headerAttributes );
    {
      ScopedXmlTag vtkFileTag( output, "VTKFile", headerAttributes );
      {
        ScopedXmlTag pUnstructuredGridFileTag( output, "PUnstructuredGrid", { { "GhostLevel", std::to_string( ghostLevel ) } });
        {
          ScopedXmlTag pPointDataTag( output, "PPointData", { } );

          for( const auto& dataSet : pointData )
          {
            parallelHelper::addPEmptyDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ));
          }
        } // PPointData
        {
          ScopedXmlTag pCellDataTag( output, "PCellData", { } );

          for( const auto& dataSet : cellData )
          {
            parallelHelper::addPEmptyDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ) );
          }
        } // PCellData
        {
          ScopedXmlTag pPointsTag( output, "PPoints", {} );
          parallelHelper::addPEmptyDataSet( writer, output, points, 3, "" );
        } // PPoints
        for( size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles )
        {
          std::string pieceName = baseName + "/" + baseName + "_" + std::to_string( nFiles ) + ".vtu";
          writeEmptyTag( output, "Piece", { { "Source", pieceName } } );
        } // Pieces
      } // PUnstructuredGrid
    } // VTKFile
    output.close( );
  } // writePVTUfile
namespace parallelHelper
{
  // NOTES:Add this function to addDataSet via another input argument?
  template<typename Writer, typename DataType>
  inline void addPEmptyDataSet( Writer& writer,
                                std::ostream& output,
                                const std::vector<DataType>& data,
                                size_t numberOfComponents,
                                const std::string& name)
  {
    StringStringMap attributes = { { "type", dataTypeString<DataType>() } };

    if( numberOfComponents > 1 )
    {
      attributes["NumberOfComponents"] = std::to_string( numberOfComponents );
    }

    if ( name != "" )
    {
      attributes["Name"] = name;
    }

    writer.addDataAttributes( attributes );

    writeEmptyTag( output, "PDataArray", attributes );
  } // addPEmptyDataSet
} // namespace parallelHelper
} // namespace vtu11
#endif //VTU11_PARALLEL_HELPER_IMPL_HPP
