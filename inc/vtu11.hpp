#ifndef VTU11_VTU11_HPP
#define VTU11_VTU11_HPP

#include "alias.hpp"
#include "writer.hpp"

namespace vtu11
{

struct UnstructuredMesh
{
  std::vector<double>& points;
  std::vector<size_t>& connectivity;
  std::vector<size_t>& offsets;
  std::vector<VtkCellType>& types;
};

template<typename Writer = AsciiWriter>
void write( std::ostream& output, 
            const UnstructuredMesh& mesh,
            const std::vector<DataSet>& pointData,
            const std::vector<DataSet>& cellData );
            
} // namespace vtu11

#include "vtu11_impl.hpp"

#endif // VTU11_VTU11_HPP
