#          __        ____ ____
# ___  ___/  |_ __ _/_   /_   |
# \  \/ /\   __\  |  \   ||   |
#  \   /  |  | |  |  /   ||   |
#   \_/   |__| |____/|___||___|
#
#  License: BSD License ; see LICENSE
#

cmake_minimum_required( VERSION 3.12 )

project( vtu11 CXX )

# --------------- setup vtu11 interface library ----------------

add_library( vtu11 INTERFACE )
add_library( vtu11::vtu11 ALIAS vtu11 )

target_include_directories( vtu11 INTERFACE . )
target_compile_features( vtu11 INTERFACE cxx_std_11 )

find_package( ZLIB )

if( ZLIB_FOUND )

    message( STATUS "Enabling vtu11 with zlib compression" )

    target_include_directories( vtu11 INTERFACE ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( vtu11 INTERFACE ${ZLIB_LIBRARIES} )
    target_compile_definitions( vtu11 INTERFACE VTU11_ENABLE_ZLIB )

endif( ZLIB_FOUND )

# ------------------- setup vtu11 unit tests -------------------

option( VTU11_ENABLE_TESTS "Build vtu11 unit tests." OFF )

if( ${VTU11_ENABLE_TESTS} )

    set( VTU11_HEADERS
         vtu11/vtu11.hpp
         vtu11/inc/alias.hpp
         vtu11/inc/filesystem.hpp
         vtu11/inc/utilities.hpp
         vtu11/inc/writer.hpp
         vtu11/inc/zlibWriter.hpp
         vtu11/impl/utilities_impl.hpp
         vtu11/impl/vtu11_impl.hpp
         vtu11/impl/writer_impl.hpp
         vtu11/impl/zlibWriter_impl.hpp )

    set( VTU11_TEST_SOURCES
         test/main_test.cpp
         test/pwrite_pyramids3D_test.cpp
         test/utilities_test.cpp
         test/vtu11_testing.cpp
         test/vtu11_testing.hpp
         test/write_hexahedras3D_test.cpp
         test/write_icosahedron3D_test.cpp
         test/write_pyramids3D_test.cpp
         test/write_square2D_test.cpp )

    add_executable( vtu11_testrunner ${VTU11_HEADERS} ${VTU11_TEST_SOURCES} )

    target_link_libraries( vtu11_testrunner PRIVATE vtu11::vtu11 )

    # Enable more warnings and treat warnings as errors
    if ( CMAKE_COMPILER_IS_GNUCXX )
        target_compile_options( vtu11_testrunner PRIVATE -fPIC -pedantic -Wall -Wextra -Wcast-align
            -Wsuggest-attribute=cold -Wsuggest-attribute=pure -Wimport -Wsuggest-final-methods
            -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-attribute=malloc
            -Wsuggest-attribute=noreturn -Wformat-y2k -Wpacked -Wno-restrict -Wswitch-enum -Wwrite-strings
            -Wformat-nonliteral -Wformat-security -Wcast-qual -Wsuggest-override -Wsuggest-final-types
            -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept
            -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion
            -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Werror )

    elseif( MSVC )
        # required such that "__cplusplus" is set to the correct value
        # see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
        # Note: min value is c++14 => 201402L (c++11 does not exist, will also output 201402L)
        target_compile_options( vtu11_testrunner PRIVATE /Zc:__cplusplus /W3 /EHsc /WX )

    elseif( CMAKE_CXX_COMPILER_ID MATCHES "IntelLLVM" )
        target_compile_options( vtu11_testrunner PRIVATE -Wall -Werror )

    elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
        target_compile_options( vtu11_testrunner PRIVATE -Wall -Werror-all )

    else( CMAKE_COMPILER_IS_GNUCXX )
        target_compile_options( vtu11_testrunner PRIVATE -Wall -Werror )

    endif ( CMAKE_COMPILER_IS_GNUCXX )

    include(CTest)

    include("${CMAKE_CURRENT_SOURCE_DIR}/test/catch2/Catch.cmake")

    catch_discover_tests( vtu11_testrunner )

    file( COPY test/testfiles DESTINATION . )

endif( ${VTU11_ENABLE_TESTS} )