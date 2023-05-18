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

#include "vtu11/inc/alias.hpp"

#include <functional>
#include <limits>
#include <type_traits>
#include <array>

namespace vtu11
{

#define VTU11_THROW( message ) throw std::runtime_error( message )
#define VTU11_CHECK( expr, message ) if( !( expr ) ) VTU11_THROW ( message )

std::string endianness( );

class Base64EncodedOutput
{
public:
    template<typename TIteratorType>
    void writeOutputData( std::ostream& output,
                          TIteratorType begin,
                          size_t n );

    void closeOutputData( std::ostream& output );

private:
    static void encodeTriplet( std::ostream& output,
                               const std::array<char, 3>& bytes,
                               size_t padding );

    size_t byteTripletIndex = 0;

    std::array<char, 3> byteTriplet;
};

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

template<typename T> inline
std::string appendSizeInBits( const char* str )
{
    return str + std::to_string( sizeof( T ) * 8 );
}

// SFINAE if signed integer
template<typename T> inline
typename std::enable_if<std::numeric_limits<T>::is_integer &&
                        std::numeric_limits<T>::is_signed, std::string>::type
    dataTypeString( )
{
    return appendSizeInBits<T>( "Int" );
}

// SFINAE if unsigned signed integer
template<typename T> inline
typename std::enable_if<std::numeric_limits<T>::is_integer &&
                       !std::numeric_limits<T>::is_signed, std::string>::type
    dataTypeString( )
{
    return appendSizeInBits<T>( "UInt" );
}

// SFINAE if double or float
template<typename T> inline
typename std::enable_if<std::is_same<T, double>::value ||
                        std::is_same<T, float>::value, std::string>::type
    dataTypeString( )
{
    return appendSizeInBits<T>( "Float" );
}

} // namespace vtu11

#include "vtu11/impl/utilities_impl.hpp"

#endif // VTU11_UTILITIES_HPP
