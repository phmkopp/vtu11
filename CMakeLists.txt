#          __        ____ ____
# ___  ___/  |_ __ _/_   /_   |
# \  \/ /\   __\  |  \   ||   |
#  \   /  |  | |  |  /   ||   |
#   \_/   |__| |____/|___||___|
#
#  License: BSD License ; see LICENSE
#


cmake_minimum_required( VERSION 3.4.3 )

project( vtu11 CXX )

set( CMAKE_CXX_STANDARD 11 )

include(CTest)

OPTION ( VTU11_STRICT_COMPILER "Enable more warnings as errors." OFF )

# If compiler is g++: Enable further  warnings and treat all warnings as errors
if( VTU11_STRICT_COMPILER )
    if ( CMAKE_COMPILER_IS_GNUCXX )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pedantic -Wall -Wextra -Wcast-align -Wsuggest-attribute=cold       \
            -Wsuggest-attribute=pure -Wimport -Wsuggest-final-methods -Wsuggest-attribute=const -Wsuggest-attribute=format \
            -Wsuggest-attribute=malloc -Wsuggest-attribute=noreturn -Wformat-y2k -Wpacked -Wno-restrict                    \
            -Wswitch-enum -Wwrite-strings -Wformat-nonliteral -Wformat-security -Wcast-qual -Wsuggest-override             \
            -Wsuggest-final-types -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs       \
            -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo     \
            -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Werror" )

    elseif( MSVC )
        # required such that "__cplusplus" is set to the correct value
        # see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
        # Note: min value is c++14 => 201402L (c++11 does not exist, will also output 201402L)
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /W3 /EHsc /WX" )

    elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror-all")

    else( CMAKE_COMPILER_IS_GNUCXX )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

    endif ( CMAKE_COMPILER_IS_GNUCXX )
endif( VTU11_STRICT_COMPILER )

file( GLOB TEST_FILES test/*.cpp)
file( GLOB_RECURSE INCLUDE_FILES vtu11/*.hpp)

add_executable( vtu11_testrunner ${TEST_FILES} ${INCLUDE_FILES} test/vtu11_testing.hpp )

target_include_directories( vtu11_testrunner PRIVATE . )

find_package( ZLIB )

if( ZLIB_FOUND )

    message( STATUS "Enabling vtu11 tests with zlib compression" )

    target_link_libraries( vtu11_testrunner PRIVATE ${ZLIB_LIBRARIES} )
    target_include_directories( vtu11_testrunner PRIVATE ${ZLIB_INCLUDE_DIRS} )
    target_compile_definitions( vtu11_testrunner PRIVATE VTU11_ENABLE_ZLIB )

else( ZLIB_FOUND )

    message( STATUS "Enabling vtu11 tests without zlib compression" )

endif( ZLIB_FOUND )

include("${CMAKE_CURRENT_SOURCE_DIR}/test/catch2/Catch.cmake")

catch_discover_tests( vtu11_testrunner )

file( COPY test/testfiles DESTINATION . )

