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
#include "impl/vtu11_impl.hpp"
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
inline bool directoryExists(const std::string& directoryPath)
{
	DWORD ftyp = GetFileAttributesA(directoryPath.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
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
