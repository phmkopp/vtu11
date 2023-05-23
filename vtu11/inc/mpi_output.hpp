//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#ifndef VTU11_MPI_OUTPUT
#define VTU11_MPI_OUTPUT

#include "mpi.h"
#include "vtu11/inc/alias.hpp"

namespace vtu11
{
class MPIOutput : public std::ostream
{
public:
    MPIOutput(const std::string& filename, MPI_Comm mpicomm) : fileName(filename), mpiComm(mpicomm)
    {
        MPI_Comm_rank(mpiComm, &rank);
        MPI_Comm_size(mpiComm, &size);

        MPI_File_open(mpiComm, fileName.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fileHandle);

        std::cout << "Opening Stuff" << std::endl;
    }

    MPIOutput& operator << (const Rank r)
    {
        std::cout << "Setting the RANK!" << ((r==Rank::All) ? "All" : "Zero") << std::endl;
        rankConfig = r;
        return *this;
    }

    MPIOutput& operator << (const char* pString)
    {
        std::cout << "operator << (const char* pString)" << pString;
        return *this;
    }

    ~MPIOutput()
    {
        MPI_File_close(&fileHandle);
    }

private:
    std::string fileName;
    MPI_File fileHandle;
    MPI_Comm mpiComm;
    int rank;
    int size;
    Rank rankConfig;
};

} // namespace vtu11

#endif // VTU11_MPI_OUTPUT