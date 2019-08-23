#ifndef VTU11_WRITER_IMPL_HPP
#define VTU11_WRITER_IMPL_HPP

#include "utilities.hpp"
#include "base64.hpp"

namespace vtu11
{

template<typename T>
inline void AsciiWriter::writeData( std::ostream& output, 
                                    const std::vector<T>& data )
{
  for( auto value : data )
  {
      output << value << " ";
  }
}
  
inline void AsciiWriter::appendHeaderAttributes( StringStringMap& )
{
}

inline void AsciiWriter::appendDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "ascii";
}

template<typename T>
inline void Base64BinaryWriter::writeData( std::ostream& output, 
                                           const std::vector<T>& data )
{
  HeaderType numberOfBytes = data.size( ) * sizeof( T );

  output << base64Encode( &numberOfBytes, &numberOfBytes + 1 );
  output << base64Encode( data.begin( ), data.end( ) );
}
    
inline void Base64BinaryWriter::appendHeaderAttributes( StringStringMap& attributes )
{
  attributes["header_type"] = dataTypeString<HeaderType>( );
}

inline void Base64BinaryWriter::appendDataAttributes( StringStringMap& attributes )
{
  attributes["format"] = "binary";
}

} // namespace vtu11

#endif // VTU11_WRITER_IMPL_HPP
