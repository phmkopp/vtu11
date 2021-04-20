# Vtu11

_Vtu11_ is a small C++ header-only library to write unstructured grids using the vtu file format. It keeps the mess of dealing with file writing in different formats away from you. Currently it does not add any features for setting up the required data structure because this vastly differs based on the context in which _vtu11_ is used.

## Serial example

```cpp
#include "vtu11/vtu11.hpp"

int main( )
{
    // Create data for 3x2 quad mesh: (x, y, z) coordinates of mesh vertices
    std::vector<double> points
    {
        0.0, 0.0, 0.5,    0.0, 0.3, 0.5,    0.0, 0.7, 0.5,    0.0, 1.0, 0.5, // 0,  1,  2,  3
        0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
        1.0, 0.0, 0.5,    1.0, 0.3, 0.5,    1.0, 0.7, 0.5,    1.0, 1.0, 0.5  // 8,  9, 10, 11
    };

    // Vertex indices of all cells
    std::vector<vtu11::VtkIndexType> connectivity
    {
        0,  4,  5,  1, // 0
        1,  5,  6,  2, // 1
        2,  6,  7,  3, // 2
        4,  8,  9,  5, // 3
        5,  9, 10,  6, // 4
        6, 10, 11,  7  // 5
    };

    // Separate cells in connectivity array
    std::vector<vtu11::VtkIndexType> offsets { 4, 8, 12, 16, 20, 24 };

    // Cell types of each cell, see [1]
    std::vector<vtu11::VtkCellType> types { 9, 9, 9, 9, 9, 9 };

    // Create small proxy mesh type
    vtu11::Vtu11UnstructuredMesh mesh { points, connectivity, offsets, types };

    // Create some data associated to points and cells
    std::vector<double> pointData { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
    std::vector<double> cellData { 3.2, 4.3, 5.4, 6.5, 7.6, 8.7 };

    // Create tuples with (name, association, number of components) for each data set
    std::vector<vtu11::DataSetInfo> dataSetInfo
    {
        { "Temperature", vtu11::DataSetType::PointData, 1 },
        { "Conductivity", vtu11::DataSetType::CellData, 1 },
    };

    // Write data to .vtu file using Ascii format
    vtu11::writeVtu( "test.vtu", mesh, dataSetInfo, { pointData, cellData }, "Ascii" );
}
```
Available writers are (not case sensitive):
- `"Ascii"`
- `"Base64Inline"`
- `"Base64Appended"`
- `"RawBinary"`
- `"RawBinaryCompressed"`

Comments:
- RawCompressedBinary requires [zlib](https://zlib.net/) to be enabled by defining the VTU11_ENABLE_ZLIB proprocessor symbol. Otherwise the uncompressed version is used instead. Compiled executables also have to be linked to zlib.
- Compressing data takes more time than writing more data uncompressed
- Ascii produces surprisingly small files, is nice to debug, but is rather slow to read in Paraview. Archiving ascii .vtu files using a standard zip tool (for example) produces decently small file sizes.
- Writing raw binary data breakes the xml standard. To still produce valid xml files you can use base64 encoding, at the cost of having about 30% times larger files.  
- Both raw binary modes use appended format 

## How to include in your project

The lazy way of using _vtu11_ is to use the single header version provided with the latest release. If you want to use the project as it is, then you need to add it to the directories that the compiler searches for include files and compile using (at least) the C++ 11 standard. Let's say you are working in a Linux environment where you clone the _vtu11_ project and create an `example.cpp` next to it. Using for example `g++` you compile as follows:
```
g++ -Ivtu11 --std=c++11 -o example example.cpp
```
If you want to use compressed vtu output, then you can add the `VTU11_ENABLE_ZLIB` definition and link to zlib:
```
g++ -Ivtu11 -DVTU11_ENABLE_ZLIB -lz --std=c++11 -o example example.cpp
```
To be a bit more platform independent and handle the zlib part automatically we can use CMake by adding the following `CMakeLists.txt`:
```cmake
# Not sure what the earliest version would be
cmake_minimum_required( VERSION 3.12 )

project( example LANGUAGES CXX )

# Create interface target to hold properties
add_library( vtu11 INTERFACE )

target_compile_features( vtu11 INTERFACE cxx_std_11 )
target_include_directories( vtu11 INTERFACE vtu11 )

find_package( ZLIB )

if( ZLIB_FOUND )
    message( STATUS "Compiling with zlib" )

    target_include_directories( vtu11 INTERFACE ${ZLIB_INCLUDE_DIRS} )
    target_compile_definitions( vtu11 INTERFACE VTU11_ENABLE_ZLIB )
    target_link_libraries( vtu11 INTERFACE ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

# Create example and import properties from vtu11
add_executable( example example.cpp )

target_link_libraries( example PRIVATE vtu11 )
```
Now you can create a build directory, compile the project and run the example.

## Parallel example

The pvtu format is used in combination with the vtu format. The mesh needs to be partitioned before it is given to _vtu11_. Each part of the mesh is written to a vtu file, and the pvtu file contains the references to those files. Overlapping entities like ghost nodes or cells can be added too if needed for e.g. other cells.

```cpp
#include "vtu11/vtu11.hpp"

int main( )
{
    // Split the 3x2 cells from serial example in two 3x1 partitions

    // Row 0 and 1
    std::vector<double> points0
    {
        0.0, 0.0, 0.5,    0.0, 0.3, 0.5,    0.0, 0.7, 0.5,    0.0, 1.0, 0.5, // 0,  1,  2,  3
        0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
    };

    // Row 1 and 2
    std::vector<double> points1
    {                                                                        // Original indices:
        0.5, 0.0, 0.5,    0.5, 0.3, 0.5,    0.5, 0.7, 0.5,    0.5, 1.0, 0.5, // 4,  5,  6,  7
        1.0, 0.0, 0.5,    1.0, 0.3, 0.5,    1.0, 0.7, 0.5,    1.0, 1.0, 0.5  // 8,  9, 10, 11
    };

    // Original cells 0, 1 and 2
    std::vector<vtu11::VtkIndexType> connectivity0
    {
        0,  4,  5,  1, // 0
        1,  5,  6,  2, // 1
        2,  6,  7,  3, // 2
    };

    // Original cells 3, 4, and 5 (now using local vertex indices)
    std::vector<vtu11::VtkIndexType> connectivity1
    {
        0,  4,  5,  1, // 3
        1,  5,  6,  2, // 4
        2,  6,  7,  3, // 5
    };

    std::vector<vtu11::VtkIndexType> offsets0 { 4, 8, 12 };
    std::vector<vtu11::VtkIndexType> offsets1 { 4, 8, 12 };

    std::vector<vtu11::VtkCellType> types0 { 9, 9, 9 };
    std::vector<vtu11::VtkCellType> types1 { 9, 9, 9 };

    // Create one proxy for each partition
    vtu11::Vtu11UnstructuredMesh meshPartition0 { points0, connectivity0, offsets0, types0 };
    vtu11::Vtu11UnstructuredMesh meshPartition1 { points1, connectivity1, offsets1, types1 };

    // Because vertices are duplicated also point data is duplicated
    std::vector<double> pointData0 { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
    std::vector<double> pointData1 { 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };

    std::vector<double> cellData0 { 3.2, 4.3, 5.4 };
    std::vector<double> cellData1 { 6.5, 7.6, 8.7 };

    // These hold for all partitions
    std::vector<vtu11::DataSetInfo> dataSetInfo
    {
        { "Temperature", vtu11::DataSetType::PointData, 1 },
        { "Conductivity", vtu11::DataSetType::CellData, 1 },
    };

    std::vector<vtu11::DataSetData> dataSetData0 { pointData0, cellData0 };
    std::vector<vtu11::DataSetData> dataSetData1 { pointData1, cellData1 };

    size_t numberOfFiles = 2;

    std::string path = ".";
    std::string basename = "test";

    // First write .pvtu file and create folder for .vtu partitions. This must be
    // done only once (e.g. on MPI rank 0 or before an omp parallel section).
    vtu11::writePVtu( path, basename, dataSetInfo, numberOfFiles );

    // Write two .vtu partitions. This can happen in parallel as there are no dependencies.
    // Note that the `writePVtu` must have completed before calling this function
    vtu11::writePartition( path, basename, meshPartition0, dataSetInfo, dataSetData0, 0, "RawBinary" );
    vtu11::writePartition( path, basename, meshPartition1, dataSetInfo, dataSetData1, 1, "RawBinary" );
}
```

The folder structure for the example above would look like this (in folder `./`):
```
test.pvtu
test
  |-- test_0.vtu
  |-- test_1.vtu
```

## Resources

[1] https://vtk.org/wp-content/uploads/2015/04/file-formats.pdf
