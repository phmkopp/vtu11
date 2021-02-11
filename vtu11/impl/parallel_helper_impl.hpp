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

#include "vtu11_impl.hpp"
namespace vtu11
{
  template<typename Writer>
  void writePVTUfile( const std::string& path,
                      const std::string& baseName,
                      const std::vector<DataSet>& pointData,
                      const std::vector<DataSet>& cellData,
                      size_t numberOfFiles,
                      Writer writer)
  {
    std::string parallelName = path + baseName + ".pvtu";
    std::ofstream output( parallelName, std::ios::binary );    
    size_t ghostLevel = 0;//Hardcoded to be 0
    //std::vector<double> points;
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
            detail::addDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ), true);
          }
        } // PPointData
        {
          ScopedXmlTag pCellDataTag( output, "PCellData", { } );

          for( const auto& dataSet : cellData )
          {
            detail::addDataSet( writer, output, std::get<2>( dataSet ), std::get<1>( dataSet ), std::get<0>( dataSet ), true );
          }
        } // PCellData
        {
          ScopedXmlTag pPointsTag( output, "PPoints", {} );
		  StringStringMap attributes = { { "type", dataTypeString<double>() }, { "NumberOfComponents", std::to_string(3) } };
		  writer.addDataAttributes(attributes);
          writeEmptyTag(output, "PDataArray", attributes);
          // detail::addDataSet( writer, output, points, 3, "", true );
        } // PPoints
        for( size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles )
        {
          std::string pieceName = baseName + "/" + baseName + "_" + std::to_string( nFiles ) + ".vtu";
          writeEmptyTag( output, "Piece", { { "Source", pieceName } } );
        } // Pieces
      } // PUnstructuredGrid
    } // PVTUFile
    output.close( );
  } // writePVTUfile
} // namespace vtu11
#endif //VTU11_PARALLEL_HELPER_IMPL_HPP
