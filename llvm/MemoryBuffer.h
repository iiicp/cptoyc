/**********************************
* File:     MemoryBuffer.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/24
***********************************/

#ifndef CLANGSIMPLIFY_MEMORYBUFFER_H
#define CLANGSIMPLIFY_MEMORYBUFFER_H
#include <iostream>
namespace CPToyC {
    namespace Compiler {
        class MemoryBuffer {
            const char *BufferStart;
            const char *BufferEnd;

            bool MustDeleteBuffer;

        protected:
            MemoryBuffer() : MustDeleteBuffer(false) {}

            void init(const char *BufStart, const char *BufEnd);

            void initCopyOf(const char *BufStart, const char *BufEnd);

        public:
            virtual ~MemoryBuffer();

            const char *getBufferStart() const { return BufferStart; }

            const char *getBufferEnd() const { return BufferEnd; }

            size_t getBufferSize() const { return BufferEnd - BufferStart; }

            virtual const char *getBufferIdentifier() const {
                return "Unknown buffer";
            }

            static MemoryBuffer *getFile(const char *Filename, std::string *ErrStr = 0, int64_t FileSize = -1);

            static MemoryBuffer *getMemBuffer(const char *StartPtr, const char *EndPtr, const char *BufferName = "");

            static MemoryBuffer *
            getMemBufferCopy(const char *StartPtr, const char *EndPtr, const char *BufferName = "");

            static MemoryBuffer *getNewMemBuffer(size_t Size, const char *BufferName = "");

            static MemoryBuffer *getNewUninitMemBuffer(size_t Size, const char *BufferName = "");

            static MemoryBuffer *getSTDIN();

            static MemoryBuffer *getFileOrSTDIN(const char *FileName, std::string *ErrStr = 0, int64_t FileSize = -1);

            static MemoryBuffer *getFileOrSTDIN(const std::string &FN, std::string *ErrStr = 0, int64_t FileSize = -1) {
                return getFileOrSTDIN(FN.c_str(), ErrStr, FileSize);
            }
        };
    }
}

#endif//CLANGSIMPLIFY_MEMORYBUFFER_H
