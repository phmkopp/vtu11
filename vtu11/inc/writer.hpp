//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_WRITER_HPP
#define VTU11_WRITER_HPP

#include "vtu11/inc/alias.hpp"
#include "vtu11/inc/zlibWriter.hpp"

namespace vtu11
{

struct AsciiWriter
{
  template<typename T, typename Output>
  void writeData( Output& output,
                  const std::vector<T>& data );

  template<typename Output>
  void writeAppended( Output& output );

  void addHeaderAttributes( StringStringMap& attributes );
  void addDataAttributes( StringStringMap& attributes );

  StringStringMap appendedAttributes( );
};

struct Base64BinaryWriter
{
  template<typename T, typename Output>
  void writeData( Output& output,
                  const std::vector<T>& data );

  template<typename Output>
  void writeAppended( Output& output );

  void addHeaderAttributes( StringStringMap& attributes );
  void addDataAttributes( StringStringMap& attributes );

  StringStringMap appendedAttributes( );
};

struct Base64BinaryAppendedWriter
{
  template<typename T>
  void writeData( std::ostream& output,
                  const std::vector<T>& data );

  void writeAppended( std::ostream& output );

  void addHeaderAttributes( StringStringMap& attributes );
  void addDataAttributes( StringStringMap& attributes );

  StringStringMap appendedAttributes( );

  size_t offset = 0;

  std::vector<std::pair<const char*, HeaderType>> appendedData;
};

struct RawBinaryAppendedWriter
{
  template<typename T>
  void writeData( std::ostream& output,
                  const std::vector<T>& data );

  void writeAppended( std::ostream& output );

  void addHeaderAttributes( StringStringMap& attributes );
  void addDataAttributes( StringStringMap& attributes );

  StringStringMap appendedAttributes( );

  size_t offset = 0;

  std::vector<std::pair<const char*, HeaderType>> appendedData;
};

} // namespace vtu11

#include "vtu11/impl/writer_impl.hpp"

#endif // VTU11_WRITER_HPP
