//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//
#ifndef VTU11_PARALLEL_HELPER_HPP
#define VTU11_PARALLEL_HELPER_HPP

namespace vtu11
{
  //writes the .pvtu file, that keeps together all vtu pieces
  template<typename Writer>
  void writePVTUfile( const std::string & path,
                      std::string baseName,
                      const std::vector<DataSet> & pointData,
                      const std::vector<DataSet> & cellData,
                      size_t numberOfFiles,
                      Writer writer);
} // namespace vtu11
#include "impl/parallel_helper_impl.hpp"
#endif //VTU11_PARALLEL_HELPER_HPP
