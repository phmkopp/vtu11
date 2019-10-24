cmake_minimum_required( VERSION 3.4.3 )

project( vtu11 CXX )

# Directories to include header files from
include_directories( inc )
include_directories( impl )
include_directories( external/catch2 )
include_directories( external/zlib/inc )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# Write all include directories into ALL_INCLUDES variable
get_property( ALL_INCLUDES DIRECTORY PROPERTY INCLUDE_DIRECTORIES )

# Set new variable VTU11_INCLUDE_DIRECTORIES also in parent scope
set( VTU11_INCLUDE_DIRECTORIES ${ALL_INCLUDES} PARENT_SCOPE )

file( GLOB TEST_FILES test/*.cpp)

set( CMAKE_CXX_STANDARD 11 )

# This enables exporting all symbols to the dll on windows
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

# If compiler is g++: Enable further  warnings and treat all warnings as errors
if( CMAKE_COMPILER_IS_GNUCXX )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Werror -Wextra -Wshadow -Wsuggest-override -Wsuggest-final-types -fPIC" )
endif( CMAKE_COMPILER_IS_GNUCXX )

# -------------------- Set up testrunner --------------------------

OPTION ( VTU11_ENABLE_TESTS "Enable unit tests." )

if( VTU11_ENABLE_TESTS )

    # Add an executable and link to the library
    add_executable( vtu11_testrunner ${TEST_FILES} )

endif( VTU11_ENABLE_TESTS )

# -------------------- zlib dependency ---------------------------

OPTION ( VTU11_ENABLE_ZLIB "Adds compressed writers using zlib compression." )

IF( VTU11_ENABLE_ZLIB )

    message( "Enabling zlib extension" )
    
    find_package( ZLIB REQUIRED )
    
    if ( ZLIB_FOUND )
        message( "Found zlib" )
        include_directories( ${ZLIB_INCLUDE_DIRS} )
        
        if( VTU11_ENABLE_TESTS )
            target_link_libraries( vtu11_testrunner ${ZLIB_LIBRARIES} )
        endif( VTU11_ENABLE_TESTS )
    
    endif( ZLIB_FOUND )

ENDIF( VTU11_ENABLE_ZLIB )

# ---------------------- Copy / install ----------------------------

file(COPY test/testfiles DESTINATION . )

CONFIGURE_FILE ( vtu11_config.hpp.in vtu11_config.hpp )

