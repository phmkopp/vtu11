//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_WRITER_IMPL_HPP
#define VTU11_WRITER_IMPL_HPP

#include "vtu11/inc/utilities.hpp"

#include <fstream>

namespace vtu11
{
namespace detail
{

template<typename T> inline
void writeNumber( char (&buffer)[64], T value )
{
    VTU11_THROW( "Invalid data type." );
}

#define __VTU11_WRITE_NUMBER_SPECIALIZATION( string, type )    \
template<> inline                                             \
void writeNumber<type>( char (&buffer)[64], type value )         \
{                                                             \
    std::snprintf( buffer, sizeof( buffer ), string, value ); \
}

__VTU11_WRITE_NUMBER_SPECIALIZATION( VTU11_ASCII_FLOATING_POINT_FORMAT, double )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%lld", long long int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%ld" , long int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%d"  , int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%hd" , short )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%hhd", char )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%llu", unsigned long long int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%ld" , unsigned long int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%d"  , unsigned int )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%hd" , unsigned short )
__VTU11_WRITE_NUMBER_SPECIALIZATION( "%hhd", unsigned char )

} // namespace detail

template<typename T>
inline void AsciiWriter::writeData( std::ostream& output,
                                    const std::vector<T>& data )
{
    char buffer[64];

    for( auto value : data )
    {
        detail::writeNumber( buffer, value );

        output << buffer << " ";
    }

    output << "\n";
}

template<>
inline void AsciiWriter::writeData( std::ostream& output,
                                    const std::vector<std::int8_t>& data )
{
  for( auto value : data )
  {
    // will otherwise interpret uint8 as char and output nonsense instead
  	// changed the datatype from unsigned to int
      output << static_cast<int>( value ) << " ";
  }

  output << "\n";
}

inline void AsciiWriter::writeAppended( std::ostream& )
{

}

inline void AsciiWriter::addHeaderAttributes( StringStringMap& )
{
}

inline void AsciiWriter::addDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "ascii";
}

inline StringStringMap AsciiWriter::appendedAttributes( )
{
  return { };
}

// ----------------------------------------------------------------

template<typename T>
inline void Base64BinaryWriter::writeData( std::ostream& output,
                                           const std::vector<T>& data )
{
  HeaderType numberOfBytes = data.size( ) * sizeof( T );

  output << base64Encode( &numberOfBytes, &numberOfBytes + 1 );
  output << base64Encode( data.begin( ), data.end( ) );

  output << "\n";
}

inline void Base64BinaryWriter::writeAppended( std::ostream& )
{

}

inline void Base64BinaryWriter::addHeaderAttributes( StringStringMap& attributes )
{
  attributes["header_type"] = dataTypeString<HeaderType>( );
}

inline void Base64BinaryWriter::addDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "binary";
}

inline StringStringMap Base64BinaryWriter::appendedAttributes( )
{
  return { };
}

// ----------------------------------------------------------------

template<typename T>
inline void Base64BinaryAppendedWriter::writeData( std::ostream&,
                                                   const std::vector<T>& data )
{
  HeaderType rawBytes = data.size( ) * sizeof( T );

  appendedData.emplace_back( reinterpret_cast<const char*>( &data[0] ), rawBytes );

  offset += encodedNumberOfBytes( rawBytes + sizeof( HeaderType ) );
}

inline void Base64BinaryAppendedWriter::writeAppended( std::ostream& output )
{
  for( auto dataSet : appendedData )
  {
    // looks like header and data has to be encoded at once
    std::vector<char> data( dataSet.second + sizeof( HeaderType ) );

    *reinterpret_cast<HeaderType*>( &data[0] ) = dataSet.second;

    std::copy( dataSet.first, dataSet.first + dataSet.second, &data[ sizeof( HeaderType ) ] );

    output << base64Encode( data.begin( ), data.end( ) );
  }

  output << "\n";
}

inline void Base64BinaryAppendedWriter::addHeaderAttributes( StringStringMap& attributes )
{
  attributes["header_type"] = dataTypeString<HeaderType>( );
}

inline void Base64BinaryAppendedWriter::addDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "appended";
  attributes["offset"] = std::to_string( offset );
}

inline StringStringMap Base64BinaryAppendedWriter::appendedAttributes( )
{
  return { { "encoding", "base64" } };
}

// ----------------------------------------------------------------

template<typename T>
inline void RawBinaryAppendedWriter::writeData( std::ostream&,
                                                const std::vector<T>& data )
{
  HeaderType rawBytes = data.size( ) * sizeof( T );

  appendedData.emplace_back( reinterpret_cast<const char*>( &data[0] ), rawBytes );

  offset += sizeof( HeaderType ) + rawBytes;
}

inline void RawBinaryAppendedWriter::writeAppended( std::ostream& output )
{
  for( auto dataSet : appendedData )
  {
    const char* headerBegin = reinterpret_cast<const char*>( &dataSet.second );

    for( const char* ptr = headerBegin; ptr < headerBegin + sizeof( HeaderType ); ++ptr )
    {
      output << *ptr;
    }

    for( const char* ptr = dataSet.first; ptr < dataSet.first + dataSet.second; ++ptr )
    {
      output << *ptr;
    }
  }

  output << "\n";
}

inline void RawBinaryAppendedWriter::addHeaderAttributes( StringStringMap& attributes )
{
  attributes["header_type"] = dataTypeString<HeaderType>( );
}

inline void RawBinaryAppendedWriter::addDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "appended";
  attributes["offset"] = std::to_string( offset );
}

inline StringStringMap RawBinaryAppendedWriter::appendedAttributes( )
{
  return { { "encoding", "raw" } };
}

} // namespace vtu11

#endif // VTU11_WRITER_IMPL_HPP
