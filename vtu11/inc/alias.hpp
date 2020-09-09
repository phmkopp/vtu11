//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_ALIAS_HPP
#define VTU11_ALIAS_HPP

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <array>


namespace vtu11
{

using StringStringMap = std::map<std::string, std::string>;

using DataSet = std::tuple<std::string, size_t, std::vector<double>>;

using VtkCellType = std::int8_t;

using HeaderType = size_t;

using Byte = unsigned char;
	
using VtkIndexType = std::int64_t;

} // namespace vtu11

// To dynamically select std::filesystem where available, you could use:
#if defined(__cplusplus) && __cplusplus >= 201703L
    #if __has_include(<filesystem>) // has_include is C++17
        #include <filesystem>
        namespace fs = std::filesystem;
    #elif __has_include(<experimental/filesystem>)
        #include <experimental/filesystem>
        namespace fs = std::experimental::filesystem;
    #else
        #include "external/filesystem/filesystem.hpp"
        namespace fs = ghc::filesystem;
    #endif
#else
    #include "external/filesystem/filesystem.hpp"
    namespace fs = ghc::filesystem;
#endif

#endif // VTU11_ALIAS_HPP
