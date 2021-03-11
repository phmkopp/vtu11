# Vtu 11

Vtu11 is a small project to write unstructured grids using the vtu file format. It keeps the mess of dealing with file writing in different formats away from you. Currently it does not add any features for setting up the required data structure because this vastly differs based on the context in which vtu11 is used.

## Small example

```cpp
#include "vtu11.hpp"

int main( )
{
    // Create data for 3x2 quad mesh
    std::vector<double> points
    {
        0.0, 0.0, 0.5,    0.0, 0.3, 0.5,    0.0, 0.7, 0.5,    0.0, 1.0, 0.5, // 0,  1,  2,  3
        0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
        1.0, 0.0, 0.5,    1.0, 0.3, 0.5,    1.0, 0.7, 0.5,    1.0, 1.0, 0.5  // 8,  9, 10, 11
    };

    std::vector<vtu11::VtkIndexType> connectivity
    {
        0,  4,  5,  1, // 0
        1,  5,  6,  2, // 1
        2,  6,  7,  3, // 2
        4,  8,  9,  5, // 3
        5,  9, 10,  6, // 4
        6, 10, 11,  7  // 5
    };

    std::vector<vtu11::VtkIndexType> offsets { 4, 8, 12, 16, 20, 24 };
    std::vector<vtu11::VtkCellType> types { 9, 9, 9, 9, 9, 9 };

    std::vector<double> pointData { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
    std::vector<double> cellData { 3.2, 4.3, 5.4, 6.5, 7.6, 8.7 };

    // Create tuples with (name, number of components, data)
    vtu11::DataSet pointDataSet { std::string( "Temperature" ), 1, pointData };
    vtu11::DataSet cellDataSet { std::string( "Conductivity" ), 1, cellData };

    // Create small proxy mesh type 
    vtu11::Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    // Write data to .vtu file using raw binary appended format
    vtu11::write( "test.vtu", mesh, { pointDataSet }, { cellDataSet }, vtu11::RawBinaryAppendedWriter { } );
}
```
Other writers are `AsciiWriter`, `Base64BinaryWriter`, `Base64BinaryAppendedWriter`, `RawBinaryAppendedWriter` and if zlib is available also `CompressedRawBinaryAppendedWriter`.
