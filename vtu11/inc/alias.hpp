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

namespace vtu11
{

using StringStringMap = std::map<std::string, std::string>;

enum class DataSetType : int
{
    PointData = 0, CellData = 1
};

using DataSetInfo = std::tuple<std::string, DataSetType, size_t>;
using DataSetData = std::vector<double>;

using VtkCellType = std::int8_t;
using VtkIndexType = std::int64_t;

using HeaderType = size_t;
using Byte = unsigned char;

} // namespace vtu11

#ifndef VTU11_ASCII_FLOATING_POINT_FORMAT
    #define VTU11_ASCII_FLOATING_POINT_FORMAT "%.6g"
#endif

// To dynamically select std::filesystem where available, you could use:
#if defined(__cplusplus) && __cplusplus >= 201703L
    #if __has_include(<filesystem>) // has_include is C++17
        #include <filesystem>
        namespace vtu11fs = std::filesystem;
    #elif __has_include(<experimental/filesystem>)
        #include <experimental/filesystem>
        namespace vtu11fs = std::experimental::filesystem;
    #else
        #include "inc/filesystem.hpp"
        namespace vtu11fs = ghc::filesystem;
    #endif
#else
    #include "vtu11/inc/filesystem.hpp"
    namespace vtu11fs = ghc::filesystem;
#endif

#endif // VTU11_ALIAS_HPP
