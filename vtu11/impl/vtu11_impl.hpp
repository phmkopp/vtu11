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

#include "vtu11/inc/utilities.hpp"

#include <limits>

namespace vtu11
{
namespace detail
{

template<typename DataType, typename Writer> inline
StringStringMap writeDataSetHeader( Writer&& writer,
                                    const std::string& name,
                                    size_t ncomponents )
{
    StringStringMap attributes = { { "type", dataTypeString<DataType>( ) } };

    if( name != "" )
    {
        attributes["Name"] = name;
    }

    if( ncomponents > 1 )
    {
        attributes["NumberOfComponents"] = std::to_string( ncomponents );
    }

    writer.addDataAttributes( attributes );

    return attributes;
}

template<typename Writer, typename DataType> inline
void writeDataSet( Writer& writer,
                   std::ostream& output,
                   const std::string& name,
                   size_t ncomponents,
                   const std::vector<DataType>& data )
{
    auto attributes = writeDataSetHeader<DataType>( writer, name, ncomponents );

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

template<typename Writer> inline
void writeDataSets( const std::vector<DataSetInfo>& dataSetInfo,
                    const std::vector<DataSetData>& dataSetData,
                    std::ostream& output, Writer& writer, DataSetType type )
{
    for( size_t iDataset = 0; iDataset < dataSetInfo.size( ); ++iDataset )
    {
        const auto& metadata = dataSetInfo[iDataset];

        if( std::get<1>( metadata ) == type )
        {
            detail::writeDataSet( writer, output, std::get<0>( metadata ), 
                std::get<2>( metadata ), dataSetData[iDataset] );
        }
    }
}

template<typename Writer> inline
void writeDataSetPVtuHeaders( const std::vector<DataSetInfo>& dataSetInfo,
                              std::ostream& output, Writer& writer, DataSetType type )
{
    for( size_t iDataset = 0; iDataset < dataSetInfo.size( ); ++iDataset )
    {
        const auto& metadata = dataSetInfo[iDataset];

        if( std::get<1>( metadata ) == type )
        {
            auto attributes = detail::writeDataSetHeader<double>( writer, 
               std::get<0>( metadata ), std::get<2>( metadata ) );

            writeEmptyTag( output, "PDataArray", attributes );
        }
    }
}

template<typename Writer, typename Content> inline
void writeVTUFile( const std::string& filename,
                   const char* type,
                   Writer&& writer, 
                   Content&& writeContent )
{
    std::ofstream output( filename, std::ios::binary );

    VTU11_CHECK( output.is_open( ), "Failed to open file \"" + filename + "\"" );

    // Set buffer size to 32K
    std::vector<char> buffer( 32 * 1024 );

    output.rdbuf( )->pubsetbuf( buffer.data( ), static_cast<std::streamsize>( buffer.size( ) ) );

    output << "<?xml version=\"1.0\"?>\n";

    StringStringMap headerAttributes { { "byte_order",  endianness( ) },
                                       { "type"      ,  type          },
                                       { "version"   ,  "0.1"         } };

    writer.addHeaderAttributes( headerAttributes );

    {
        ScopedXmlTag vtkFileTag( output, "VTKFile", headerAttributes );
        
        writeContent( output );

    } // VTKFile

    output.close( );
}

template<typename MeshGenerator, typename Writer> inline
void writeVtu( const std::string& filename,
               MeshGenerator& mesh,
               const std::vector<DataSetInfo>& dataSetInfo,
               const std::vector<DataSetData>& dataSetData,
               Writer&& writer )
{
    detail::writeVTUFile( filename, "UnstructuredGrid", writer, [&]( std::ostream& output )
    {
        {
            ScopedXmlTag unstructuredGridFileTag( output, "UnstructuredGrid", { } );
            {
                ScopedXmlTag pieceTag( output, "Piece", 
                { 
                    { "NumberOfPoints", std::to_string( mesh.numberOfPoints( ) ) },
                    { "NumberOfCells" , std::to_string( mesh.numberOfCells( )  ) } 

                } );

                {
                    ScopedXmlTag pointDataTag( output, "PointData", { } );

                    detail::writeDataSets( dataSetInfo, dataSetData, 
                        output, writer, DataSetType::PointData );

                } // PointData

                {
                    ScopedXmlTag cellDataTag( output, "CellData", { } );

                    detail::writeDataSets( dataSetInfo, dataSetData, 
                        output, writer, DataSetType::CellData );

                } // CellData

                {
                    ScopedXmlTag pointsTag( output, "Points", { } );

                    detail::writeDataSet( writer, output, "", 3, mesh.points( ) );

                } // Points

                {
                    ScopedXmlTag pointsTag( output, "Cells", { } );

                    detail::writeDataSet( writer, output, "connectivity", 1, mesh.connectivity( ) );
                    detail::writeDataSet( writer, output, "offsets", 1, mesh.offsets( ) );
                    detail::writeDataSet( writer, output, "types", 1, mesh.types( ) );

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

    } ); // writeVTUFile
    
} // writeVtu

} // namespace detail

template<typename MeshGenerator> inline
void writeVtu( const std::string& filename,
               MeshGenerator& mesh,
               const std::vector<DataSetInfo>& dataSetInfo,
               const std::vector<DataSetData>& dataSetData,
               const std::string& writeMode )
{
    auto mode = writeMode;

    std::transform( mode.begin( ), mode.end ( ), mode.begin( ), []( unsigned char c )
                    { return static_cast<unsigned char>( std::tolower( c ) ); } );

    if( mode == "ascii" )
    {
        detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, AsciiWriter { } );
    }
    else if( mode == "base64inline" )
    {
        detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, Base64BinaryWriter { } );
    }
    else if( mode == "base64appended" )
    {
        detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, Base64BinaryAppendedWriter { } );
    }
    else if( mode == "rawbinary" )
    {
        detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, RawBinaryAppendedWriter { } );
    }
    else if( mode == "rawbinarycompressed" )
    {
        #ifdef VTU11_ENABLE_ZLIB
            detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, CompressedRawBinaryAppendedWriter { } );
        #else
            detail::writeVtu( filename, mesh, dataSetInfo, dataSetData, RawBinaryAppendedWriter { } );
        #endif
    }
    else
    {
        VTU11_THROW( "Invalid write mode: \"" + writeMode + "\"." );
    }

} // writeVtu

namespace detail
{

struct PVtuDummyWriter
{
    void addHeaderAttributes( StringStringMap& ) { }
    void addDataAttributes( StringStringMap& ) { }
};

} // detail

inline void writePVtu( const std::string& path,
                       const std::string& baseName,
                       const std::vector<DataSetInfo>& dataSetInfo,
                       const size_t numberOfFiles )
{
    auto directory = vtu11fs::path { path } / baseName;
    auto pvtufile = vtu11fs::path { path } / ( baseName + ".pvtu" );

    // create directory for vtu files if not existing
    if( !vtu11fs::exists( directory ) )
    {
        vtu11fs::create_directories( directory );
    }

    detail::PVtuDummyWriter writer;

    detail::writeVTUFile( pvtufile.string( ), "PUnstructuredGrid", writer,
                          [&]( std::ostream& output )
    {
        std::string ghostLevel = "0"; // Hardcoded to be 0
            
        ScopedXmlTag pUnstructuredGridFileTag( output, 
            "PUnstructuredGrid", { { "GhostLevel", ghostLevel } } );

        {
            ScopedXmlTag pPointDataTag( output, "PPointData", { } );

            detail::writeDataSetPVtuHeaders( dataSetInfo, output, writer, DataSetType::PointData );

        } // PPointData

        {
            ScopedXmlTag pCellDataTag( output, "PCellData", { } );

            detail::writeDataSetPVtuHeaders( dataSetInfo, output, writer, DataSetType::CellData );

        } // PCellData

        {
            ScopedXmlTag pPointsTag( output, "PPoints", { } );
            StringStringMap attributes = { { "type", dataTypeString<double>( ) }, { "NumberOfComponents", "3" } };

            writer.addDataAttributes( attributes );

            writeEmptyTag( output, "PDataArray", attributes );

        } // PPoints

        for( size_t nFiles = 0; nFiles < numberOfFiles; ++nFiles )
        {
            std::string pieceName = baseName + "/" + baseName + "_" + std::to_string( nFiles ) + ".vtu";

            writeEmptyTag( output, "Piece", { { "Source", pieceName } } );

        } // Pieces

    } ); // writeVTUFile

} // writePVtu

template<typename MeshGenerator> inline
void writePartition( const std::string& path,
                     const std::string& baseName,
                     MeshGenerator& mesh,
                     const std::vector<DataSetInfo>& dataSetInfo,
                     const std::vector<DataSetData>& dataSetData,
                     size_t fileId,
                     const std::string& writeMode )
{
    auto vtuname = baseName + "_" + std::to_string( fileId ) + ".vtu";

    auto fullname = vtu11fs::path { path } / 
                    vtu11fs::path { baseName } / 
                    vtu11fs::path { vtuname };

    writeVtu( fullname.string( ), mesh, dataSetInfo, dataSetData, writeMode );

} // writePartition

} // namespace vtu11

#endif // VTU11_VTU11_IMPL_HPP
