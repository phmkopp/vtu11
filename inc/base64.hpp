#ifndef VTU11_BASE64_HPP
#define VTU11_BASE64_HPP

#include <string>

namespace vtu11
{

template<typename Iterator>
std::string base64Encode( Iterator begin, Iterator end );

} // namespace vtu11

#include "base64_impl.hpp"

#endif // VTU11_BASE64_HPP
