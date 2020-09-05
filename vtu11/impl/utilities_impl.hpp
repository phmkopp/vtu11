//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_UTILITIES_IMPL_HPP
#define VTU11_UTILITIES_IMPL_HPP

#include <limits>
#include "inc/xml.hpp"
#include "vtu11_impl.hpp"
namespace vtu11
{

template<typename DataType>
inline std::string dataTypeString( )
{
  std::string base;

  if( std::numeric_limits<DataType>::is_integer && std::numeric_limits<DataType>::is_signed )
  {
      base = "Int";
  }
  else if( std::numeric_limits<DataType>::is_integer && !std::numeric_limits<DataType>::is_signed )
  {
      base = "UInt";
  }
  else
  {
    base = "Float";
  }

  return base + std::to_string( sizeof( DataType ) * 8 );
}

// Declaration of writePVTUfile(path, baseName, fileId, numberOfFiles);
//numberOfFiles returns the number of all created vtu pieces
//fileId returns the actual ghost level?
template<typename Writer>
void writePVTUfile( const std::string& path,
                    const std::string& baseName,
                    const std::vector<DataSet>& pointData,
                    const std::vector<DataSet>& cellData,
                    size_t fileId, size_t numberOfFiles,
                    Writer writer)
{
  std::string parallelName = path + baseName + ".pvtu";
      std::ofstream output(parallelName, std::ios::binary);

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
                detail::addPEmptyDataSet(writer, output, std::get<2>(dataSet) ,std::get<1>(dataSet), std::get<0>(dataSet));
              }
            } // PPointData
            {
              ScopedXmlTag pCellDataTag(output, "PCellData", { });

              for (const auto& dataSet : cellData)
              {
                detail::addPEmptyDataSet(writer, output, std::get<2>(dataSet) ,std::get<1>(dataSet), std::get<0>(dataSet));
              }
            } // PCellData
            
            // ToDo: -absolute path or relative path? Currently relative path, results appear in build folder.
            //       -should we store this somehow? And reuse for the processes to write. I think not, we can create the name on the fly
            //       -Janina: I think relative path should be all right in the beginning, we can still change it later though
            for( size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles )
            {
              std::string pieceName = path + baseName + "/" + baseName + "_" + std::to_string( nFiles ) + ".vtu";
              writeEmptyTag(output, "Piece", { { "Source", pieceName } });
            } // Pieces

          } // PUnstructuredGrid
        } // VTKFile

        output.close();
}

} // namespace vtu11

inline std::string vtu11::endianness( )
{
   int i = 0x0001;

   if( *reinterpret_cast<char*>( &i ) != 0 )
   {
     return "LittleEndian";
   }
   else
   {
     return "BigEndian";
   }
}

#endif // VTU11_UTILITIES_IMPL_HPP
