//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_ZLIBWRITER_HPP
#define VTU11_ZLIBWRITER_HPP

#include "vtu11/inc/alias.hpp"

#ifdef VTU11_ENABLE_ZLIB

namespace vtu11
{

struct CompressedRawBinaryAppendedWriter
{
  template<typename T>
  void writeData( std::ostream& output,
                  const std::vector<T>& data );

  void writeAppended( std::ostream& output );

  void addHeaderAttributes( StringStringMap& attributes );
  void addDataAttributes( StringStringMap& attributes );

  StringStringMap appendedAttributes( );

  size_t offset = 0;

  std::vector<std::vector<std::vector<std::uint8_t>>> appendedData;
  std::vector<std::vector<HeaderType>> headers;
};

} // namespace vtu11

#include "vtu11/impl/zlibWriter_impl.hpp"

#endif // VTU11_ENABLE_ZLIB

#endif // VTU11_ZLIBWRITER_HPP
