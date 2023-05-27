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
        MPI_Comm_rank(mpiComm, &mpiRank);
        MPI_Comm_size(mpiComm, &mpiSize);

        MPI_File_open(mpiComm, fileName.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fileHandle);
    }

    MPIOutput& operator << (const Rank r)
    {
        // std::cout << "r: " << mpiRank << " | Setting the RANK: " << ((r==Rank::All) ? "All" : "Zero") << std::endl;
        rankConfig = r;
        flush(); // TODO should this flush? Probably makes sense ...
        return *this;
    }

    template <typename T>
    MPIOutput& operator << (T out)
    {
        if (mpiRank == 0 || rankConfig == Rank::All) {
            outputStream << out;
            // std::cout << "r: " << mpiRank << " | Writing to stream: " << out << std::endl;
        }

        int should_flush = size() > bufferSize;
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
            // std::cout << "r: " << mpiRank << " | Closing file, content of stream: " << outputStream.str() << std::endl;
        }
    }

    std::size_t size()
    {
        return static_cast<std::size_t>(outputStream.tellp());
    }

    std::string buffer() const
    {
        return outputStream.str();
    }

    void flush()
    {
        // std::cout << "Flushing" << std::endl;
        /*
        1. Compute own size
        2. Scan to compute offsets
        3. write_at
        4. reset stream
        */

        int stream_size = size();
        int offset;
        MPI_Scan(&stream_size, &offset, 1, MPI_INT, MPI_SUM, mpiComm);

        std::cout << "r: " << mpiRank << " | stream_size: " << stream_size << " | offset: " << offset << std::endl;

        std::cout << "r: " << mpiRank << " | filePos: " << filePos << std::endl;

        std::string data = outputStream.str(); // move is only possible with C++20
        outputStream = std::ostringstream(); // reset the stream, maybe there exists a more efficient solution. How about saving the buffer and using it directly???

        MPI_File_write_at_all(fileHandle, filePos+offset-stream_size, data.data(), data.size(), MPI_CHAR, MPI_STATUS_IGNORE);

        // saving the current position of the file (accross all ranks)
        // TODO maybe use MPI_File_seek(MPI_SEEK_END) instead, but didn't seem to work

        filePos += offset; // only required on last rank
        MPI_Bcast(
            &filePos,
            1,
            MPI_INT,
            mpiSize-1, // last rank knows the max size
            mpiComm);

        MPI_File_sync(fileHandle); // write to file
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
    int mpiRank;
    int mpiSize;
    Rank rankConfig;
    std::ostringstream outputStream;
    std::size_t bufferSize = 32000;

    int filePos = 0;


};

} // namespace vtu11

#endif // VTU11_MPI_OUTPUT