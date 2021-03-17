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

#include "inc/alias.hpp"

#include <functional>

namespace vtu11
{

#define VTU11_THROW( message ) throw std::runtime_error( message )
#define VTU11_CHECK( expr, message ) if( !( expr ) ) VTU11_THROW ( message )

template<typename DataType>
std::string dataTypeString( );

std::string endianness( );

template<typename Iterator>
std::string base64Encode( Iterator begin, Iterator end );

size_t encodedNumberOfBytes( size_t rawNumberOfBytes );

class ScopedXmlTag final
{
public:
    ScopedXmlTag( std::ostream& output,
                  const std::string& name,
                  const StringStringMap& attributes );

    ~ScopedXmlTag( );

private:
    std::function<void( )> closeTag;
};

void writeEmptyTag( std::ostream& output,
                    const std::string& name,
                    const StringStringMap& attributes );

} // namespace vtu11

#include "impl/utilities_impl.hpp"

#endif // VTU11_UTILITIES_HPP
