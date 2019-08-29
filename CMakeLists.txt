cmake_minimum_required( VERSION 3.4.3 )

project( vtu11 CXX )

# Directories to include header files from
include_directories( inc )
include_directories( impl )
include_directories( external/catch2 )
include_directories( external/zlib/inc )
include_directories( ${CMAKE_BINARY_DIR} )

file( GLOB TEST_FILES test/*.cpp)

set( CMAKE_CXX_STANDARD 11 )

# This enables exporting all symbols to the dll on windows
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

# If compiler is g++: Enable further  warnings and treat all warnings as errors
if( CMAKE_COMPILER_IS_GNUCXX )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Werror -Wextra -Wshadow -Wsuggest-override -Wsuggest-final-types -fPIC" )
endif( CMAKE_COMPILER_IS_GNUCXX )

# ----------------- Set up python bindings ------------------------
# set( PYBIND11_CPP_STANDARD -std=c++11 )
#
# # Add pybind11 project
# add_subdirectory( external/pybind11 )

# -------------------- Set up testrunner --------------------------

# Add an executable and link to the library
add_executable( vtu11_testrunner ${TEST_FILES} )

# -------------------- zlib dependency ---------------------------

OPTION ( VTU11_ENABLE_ZLIB "Adds compressed writers using zlib compression." )

IF( VTU11_ENABLE_ZLIB )

message( "Enabling zlib extension" )

find_package( ZLIB REQUIRED )

if ( ZLIB_FOUND )
    message( "Found zlib" )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( vtu11_testrunner ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

ENDIF( VTU11_ENABLE_ZLIB )

# ---------------------- Copy / install ----------------------------

file(COPY test/testfiles DESTINATION . )

CONFIGURE_FILE ( vtu11_config.hpp.in vtu11_config.hpp )

