//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#include <sstream>
#include <fstream>

namespace vtu11testing
{

std::string readFile( const std::string& filename )
{
    std::ifstream file( filename );

    if( !file.is_open( ) )
    {
        std::stringstream err_msg;
        err_msg << filename << " could not be opened!";
        throw std::runtime_error( err_msg.str( ) );
    }

    std::string contents, str;

    while( std::getline( file, str ) )
    {
        contents += str + "\n";
    }

    file.close( );

    return contents;
}

} // namespace vtu11testing
