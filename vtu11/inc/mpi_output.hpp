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

#include <sstream>

#include "mpi.h"
#include "vtu11/inc/alias.hpp"

namespace vtu11
{
class MPIOutput
{
public:
    MPIOutput(const std::string& filename, MPI_Comm mpicomm) : fileName(filename), mpiComm(mpicomm)
    {
        MPI_Comm_rank(mpiComm, &rank);
        MPI_Comm_size(mpiComm, &size);

        MPI_File_open(mpiComm, fileName.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fileHandle);
    }

    MPIOutput& operator << (const Rank r)
    {
        std::cout << "r: " << rank << " | Setting the RANK: " << ((r==Rank::All) ? "All" : "Zero") << std::endl;
        rankConfig = r;
        flush(); // TODO should this flush? Probably makes sense ...
        return *this;
    }

    template <typename T>
    MPIOutput& operator << (T out)
    {
        if (rank == 0 || rankConfig == Rank::All) {
            myStream << out;
            std::cout << "r: " << rank << " | Writing to stream: " << out << std::endl;
        }

        std::size_t stream_size = 123654; // TODO compute this

        int should_flush = stream_size > bufferSize;
        int global_flush;

        MPI_Allreduce(
            &should_flush,
            &global_flush,
            1,
            MPI_INT,
            MPI_MAX,
            mpiComm);

        if (global_flush) {
            flush();
        }

        return *this;
    }

    void close() // todo how to do this portable with ostream?
    {
        if (isOpen) {
            flush();
            MPI_File_close(&fileHandle);
            isOpen = false;
            std::cout << "r: " << rank << " | Closing file, content of stream: " << myStream.str() << std::endl;
        }
    }

    void flush()
    {
        /*
        1. Compute own size
        2. Scan to compute offsets
        3. write_at
        4. reset stream
        */

        int my_size = 963; // TODO compute this
        int offset;
        MPI_Scan(&my_size, &offset, 1, MPI_INT, MPI_SUM, mpiComm);

        // MPI_File_write_at_all(fileHandle, offset-my_size, my_data.data(), my_data.size(), MPI_CHAR, MPI_STATUS_IGNORE);

        // probably need to save the current position, or consider it while writing by MPI_FILE_POSITION or sth like that

        MPI_File_sync(fileHandle);
    }

    ~MPIOutput()
    {
        close();
    }

private:
    std::string fileName;
    MPI_File fileHandle;
    bool isOpen = true; // constucting an object also opens the file
    MPI_Comm mpiComm;
    int rank;
    int size;
    Rank rankConfig;
    std::stringstream myStream;
    std::size_t bufferSize = 32000;
};

} // namespace vtu11

#endif // VTU11_MPI_OUTPUT