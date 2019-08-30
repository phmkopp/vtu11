#ifndef VTU11_UTILITIES_HPP
#define VTU11_UTILITIES_HPP

namespace vtu11
{

template<typename DataType>
inline std::string dataTypeString( );

inline std::string endianness( );

#define VTU11_CHECK( expr, message ) if( not ( expr ) ) throw std::runtime_error( message )

} // namespace vtu11

#include "utilities_impl.hpp"

#endif // VTU11_UTILITIES_HPP
