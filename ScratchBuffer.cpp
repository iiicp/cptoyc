/**********************************
* File:     ScratchBuffer.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/2
***********************************/

#include "ScratchBuffer.h"
#include "SourceManager.h"

using namespace CPToyC::Compiler;

// ScratchBufSize - The size of each chunk of scratch memory.  Slightly less
//than a page, almost certainly enough for anything. :)
static const unsigned ScratchBufSize = 4060;

ScratchBuffer::ScratchBuffer(SourceManager &SM) : SourceMgr(SM), CurBuffer(0) {
    // Set BytesUsed so that the first call to getToken will require an alloc.
    BytesUsed = ScratchBufSize;
}

/// getToken - Splat the specified text into a temporary MemoryBuffer and
/// return a SourceLocation that refers to the token.  This is just like the
/// method below, but returns a location that indicates the physloc of the
/// token.
SourceLocation ScratchBuffer::getToken(const char *Buf, unsigned Len,
                                       const char *&DestPtr) {
    if (BytesUsed+Len+2 > ScratchBufSize)
        AllocScratchBuffer(Len+2);

    // Prefix the token with a \n, so that it looks like it is the first thing on
    // its own virtual line in caret diagnostics.
    CurBuffer[BytesUsed++] = '\n';

    // Return a pointer to the character data.
    DestPtr = CurBuffer+BytesUsed;

    // Copy the token data into the buffer.
    memcpy(CurBuffer+BytesUsed, Buf, Len);

    // Remember that we used these bytes.
    BytesUsed += Len+1;

    // Add a NUL terminator to the token.  This keeps the tokens separated, in
    // case they get relexed, and puts them on their own virtual lines in case a
    // diagnostic points to one.
    CurBuffer[BytesUsed-1] = '\0';

    return BufferStartLoc.getFileLocWithOffset(BytesUsed-Len-1);
}

void ScratchBuffer::AllocScratchBuffer(unsigned RequestLen) {
    // Only pay attention to the requested length if it is larger than our default
    // page size.  If it is, we allocate an entire chunk for it.  This is to
    // support gigantic tokens, which almost certainly won't happen. :)
    if (RequestLen < ScratchBufSize)
        RequestLen = ScratchBufSize;

    MemoryBuffer *Buf =
            MemoryBuffer::getNewMemBuffer(RequestLen, "<scratch space>");
    FileID FID = SourceMgr.createFileIDForMemBuffer(Buf);
    BufferStartLoc = SourceMgr.getLocForStartOfFile(FID);
    CurBuffer = const_cast<char*>(Buf->getBufferStart());
    BytesUsed = 1;
    CurBuffer[0] = '0';  // Start out with a \0 for cleanliness.
}
