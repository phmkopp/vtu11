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

        reset_stream();
    }

    MPIOutput& operator << (const Rank r)
    {
        rankConfig = r;
        flush();
        return *this;
    }

    template <typename T>
    MPIOutput& operator << (T out)
    {
        if (mpiRank == 0 || rankConfig == Rank::All) {
            outputStream << out;
        }

        return *this;
    }

    void close() // todo how to do this portable with ostream?
    {
        if (isOpen) {
            flush();
            MPI_File_close(&fileHandle);
            isOpen = false;
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
        int stream_size = size();
        int offset;
        MPI_Scan(&stream_size, &offset, 1, MPI_INT, MPI_SUM, mpiComm);

        std::string data = outputStream.str(); // move is only possible with C++20
        reset_stream();

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
    bool isOpen = true; // constucting an object also opens the file // TODO use this in checks
    MPI_Comm mpiComm;
    int mpiRank;
    int mpiSize;
    Rank rankConfig;
    std::ostringstream outputStream;

    int filePos = 0;

    void reset_stream()
    {
        outputStream = std::ostringstream(); // reset the stream, maybe there exists a more efficient solution. How about saving the (stringbuf???) and using it directly???
    }
};

} // namespace vtu11

#endif // VTU11_MPI_OUTPUT