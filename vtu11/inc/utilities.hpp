//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_UTILITIES_HPP
#define VTU11_UTILITIES_HPP

#include "vtu11.hpp"

namespace vtu11
{

template<typename DataType>
inline std::string dataTypeString( );

inline std::string endianness( );

#define VTU11_CHECK( expr, message ) if( !( expr ) ) throw std::runtime_error( message )

// ToDo: Check if it is ok to put this function here. It does not seem to belong here, actually
template<typename Writer = AsciiWriter>
void writePVTUfile( const std::string& path,
                    const std::string& baseName,
                    const std::vector<DataSet>& pointData,
                    const std::vector<DataSet>& cellData,
                    size_t fileId, size_t numberOfFiles,
                    Writer writer = Writer( ) );
} // namespace vtu11

#include "impl/utilities_impl.hpp"

#endif // VTU11_UTILITIES_HPP
