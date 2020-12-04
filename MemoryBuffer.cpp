/**********************************
* File:     MemoryBuffer.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/24
***********************************/

#include "MemoryBuffer.h"
#include "llvm/OwningPtr.h"
#include "llvm/SmallString.h"
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory>
#include <vector>
#include <sys/mman.h>

using namespace CPToyC::Compiler;

MemoryBuffer::~MemoryBuffer() {
    if (MustDeleteBuffer)
        free((void *)BufferStart);
}

void MemoryBuffer::initCopyOf(const char *BufStart, const char *BufEnd) {
    size_t Size = BufEnd - BufStart;
    BufferStart = (char *)malloc((Size + 1) * sizeof(char));
    BufferEnd = BufferStart + Size;
    memcpy(const_cast<char *>(BufferStart), BufStart, Size);
    *const_cast<char *>(BufferEnd) = 0; // Null terminate buffer
    MustDeleteBuffer = true;
}

void MemoryBuffer::init(const char *BufStart, const char *BufEnd) {
    assert(BufEnd[0] == 0 && "Buffer is not null terminated!");
    BufferStart = BufStart;
    BufferEnd = BufEnd;
    MustDeleteBuffer = false;
}

namespace {
    class MemoryBufferMem : public MemoryBuffer {
        std::string FileID;
    public:
        MemoryBufferMem(const char *Start, const char *End, const char *FID, bool Copy = false) : FileID(FID) {
            if (!Copy) {
                init(Start, End);
            }else {
                initCopyOf(Start, End);
            }
        }

        virtual const char *getBufferIdentifier() const override{
            return FileID.c_str();
        }
    };
}

MemoryBuffer *MemoryBuffer::getMemBuffer(const char *StartPtr, const char *EndPtr, const char *BufferName) {
    return new MemoryBufferMem(StartPtr, EndPtr, BufferName);
}

MemoryBuffer *MemoryBuffer::getMemBufferCopy(const char *StartPtr, const char *EndPtr, const char *BufferName) {
    return new MemoryBufferMem(StartPtr, EndPtr, BufferName, true);
}

MemoryBuffer *MemoryBuffer::getNewUninitMemBuffer(size_t Size, const char *BufferName) {
    char *Buf = (char *)malloc((Size + 1) * sizeof(char));
    if (!Buf) return 0;
    Buf[Size] = 0;
    MemoryBufferMem *SB = new MemoryBufferMem(Buf, Buf + Size, BufferName);
    SB->MustDeleteBuffer = true;
    return SB;
}

MemoryBuffer *MemoryBuffer::getNewMemBuffer(size_t Size, const char *BufferName) {
    MemoryBuffer *SB = getNewUninitMemBuffer(Size, BufferName);
    if (!SB) return 0;
    memset(const_cast<char *>(SB->getBufferStart()), 0, Size + 1);
    return SB;
}

MemoryBuffer *MemoryBuffer::getFileOrSTDIN(const char *FileName, std::string *ErrStr, int64_t FileSize) {
    if (FileName[0] != '-' || FileName[1] != 0) {
        return getFile(FileName, ErrStr, FileSize);
    }
    MemoryBuffer *M = getSTDIN();
    if (M) return M;

    const char *EmptyStr = "";
    return MemoryBuffer::getMemBuffer(EmptyStr, EmptyStr, "<stdin>");
}

namespace {
    const char *MapInFilePages(int FD, uint64_t FileSize) {
        int Flags = MAP_PRIVATE;
#ifdef MAP_FILE
        Flags |= MAP_FILE;
#endif
        void *BasePtr = ::mmap(0, FileSize, PROT_READ, Flags, FD, 0);
        if (BasePtr == MAP_FAILED)
            return 0;
        return (const char*)BasePtr;
    }

    void UnMapFilePages(const char *BasePtr, uint64_t FileSize) {
        ::munmap((void*)BasePtr, FileSize);
    }
}

namespace {
    class MemoryBufferMMapFile : public MemoryBuffer {
        std::string Filename;
    public:
        MemoryBufferMMapFile(const char *filename, const char *Pages, uint64_t Size)
            : Filename(filename) {
            init(Pages, Pages + Size);
        }

        virtual const char *getBufferIdentifier() const override{
            return Filename.c_str();
        }

        ~MemoryBufferMMapFile() {
            UnMapFilePages(getBufferStart(), getBufferSize());
        }
    };
}

MemoryBuffer *MemoryBuffer::getFile(const char *Filename, std::string *ErrStr, int64_t FileSize) {
    int OpenFlags = 0;
#ifdef O_BINARY
    OpenFlags |= O_BINARY;
#endif
    int FD = ::open(Filename, O_RDONLY | OpenFlags);
    if (FD == -1) {
        if (ErrStr) *ErrStr = "could not open file";
        return 0;
    }

    if (FileSize == -1) {
        struct stat FileInfo;
        if (fstat(FD, &FileInfo) == -1) {
            if (ErrStr) *ErrStr = "could not get file length";
            ::close(FD);
            return 0;
        }
        FileSize = FileInfo.st_size;
    }

    // 判断是否 FileSize == 0x1000 (FileSize & (0x1000 - 1))
    if (FileSize >= 4096*4 && (FileSize & (0x1000 - 1)) != 0) {
        if (const char *Pages = MapInFilePages(FD, FileSize)) {
            ::close(FD);
            return new MemoryBufferMMapFile(Filename, Pages, FileSize);
        }
    }

    MemoryBuffer *Buf = MemoryBuffer::getNewUninitMemBuffer(FileSize, Filename);
    if (!Buf) {
        if (ErrStr) *ErrStr = "could not allocate buffer";
        ::close(FD);
        return 0;
    }

    llvm::OwningPtr<MemoryBuffer> SB(Buf);
    char *BufPtr = const_cast<char *>(Buf->getBufferStart());

    size_t BytesLeft = FileSize;
    while (BytesLeft) {
        ssize_t NumRead = ::read(FD, BufPtr, BytesLeft);
        if (NumRead != -1) {
            BytesLeft -= NumRead;
            BufPtr += NumRead;
        }else if (errno == EINTR) {
            // try again
        }else {
            // error reading
            close(FD);
            if (ErrStr) *ErrStr = "error reading file data";
            return 0;
        }
    }
    close(FD);
    return Buf;;
}

namespace {
    class STDINBufferFile : public MemoryBuffer {
    public:
        virtual const char *getBufferIdentifier() const override{
            return "<stdin>";
        }
    };
}

MemoryBuffer *MemoryBuffer::getSTDIN() {
    char Buffer[4096*4];

    std::vector<char> FileData;

    //sys::Program::ChangeStdinToBinary();
    size_t ReadBytes;
    do {
        ReadBytes = fread(Buffer, sizeof(char), sizeof(Buffer), stdin);
        FileData.insert(FileData.end(), Buffer, Buffer + ReadBytes);
    }while (ReadBytes == sizeof(Buffer));

    FileData.push_back(0);
    size_t Size = FileData.size();
    if (Size <= 1)
        return 0;
    MemoryBuffer *B = new STDINBufferFile();
    B->initCopyOf(&FileData[0], &FileData[Size - 1]);
    return B;
}