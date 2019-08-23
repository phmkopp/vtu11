#ifndef VTU11_WRITER_HPP
#define VTU11_WRITER_HPP

#include "alias.hpp"

namespace vtu11
{

using HeaderType = size_t;

struct AsciiWriter
{
  template<typename T>
  static void writeData( std::ostream& output, 
                         const std::vector<T>& data );
                         
  static void appendHeaderAttributes( StringStringMap& attributes );
  static void appendDataAttributes( StringStringMap& attributes );
};

struct Base64BinaryWriter
{
  template<typename T>
  static void writeData( std::ostream& output, 
                         const std::vector<T>& data );
                         
  static void appendHeaderAttributes( StringStringMap& attributes );
  static void appendDataAttributes( StringStringMap& attributes );
};

            
} // namespace vtu11

#include "writer_impl.hpp"

#endif // VTU11_WRITER_HPP
