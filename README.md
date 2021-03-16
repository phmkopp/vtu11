# Vtu11

_Vtu11_ is a small C++ header-only library to write unstructured grids using the vtu file format. It keeps the mess of dealing with file writing in different formats away from you. Currently it does not add any features for setting up the required data structure because this vastly differs based on the context in which _vtu11_ is used.

## Serial example

```cpp
#include "vtu11.hpp"

int main( )
{
    // Create data for 3x2 quad mesh
    
    // (x, y, z) coordinates of mesh vertices
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
    vtu11::DataSetInfo dataSetInfo
    {
        { "Temperature", vtu11::DataSetType::PointData, 1 },
        { "Conductivity", vtu11::DataSetType::CellData, 1 },
    };
    
    // Wrap data into std::vector
    vtu11::DataSetData dataSetData { pointData, cellData };

    // Write data to .vtu file using raw binary appended format
    vtu11::write( "test.vtu", mesh, dataSetInfo, dataSetData, vtu11::RawBinaryAppendedWriter { } );
}
```
Available writers are:
- `AsciiWriter`
- `Base64BinaryWriter`
- `Base64BinaryAppendedWriter`
- `RawBinaryAppendedWriter`
- `CompressedRawBinaryAppendedWriter` (if [zlib](https://zlib.net/) is available)

## Parallel example

The pvtu format is used in combination with the vtu format. The mesh needs to be partitioned before it is given to _vtu11_. Each part of the mesh is written to a vtu file, and the pvtu file contains the references to those files. Overlapping entities like ghost nodes or cells can be added too if needed for e.g. other cells.

```cpp
#include "vtu11.hpp"

int main( )
{
    // create the mesh, pointDataSet and cellDataSet as above
    // for the local part of the partitioned mesh

    // ...

    // Write data to .vtu file using raw binary appended format
    vtu11::parallelWrite(
        "path/to/results",
        "test",
        mesh,
        { pointDataSet },
        { cellDataSet },
        0, // local process Id (e.g. "omp_get_thread_num" for OpenMP of "MPI_Comm_rank" in MPI)
        4, // number of processes (e.g. "omp_get_max_threads" for OpenMP of "MPI_Comm_size" in MPI)
        vtu11::RawBinaryAppendedWriter { }
    );
}

```

The folder structure for the example above would look like this (in folder `path/to/results`):
```
test.pvtu
test
  |-- test_0.vtu
  |-- test_1.vtu
  |-- test_2.vtu
  |-- test_3.vtu
```

## Resources

[1] https://vtk.org/wp-content/uploads/2015/04/file-formats.pdf
