/**********************************
* File:     ScratchBuffer.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/2
***********************************/

#ifndef CPTOYC_SCRATCHBUFFER_H
#define CPTOYC_SCRATCHBUFFER_H
#include "../Basic/SourceLocation.h"

namespace CPToyC {
    namespace Compiler {
        class SourceManager;
        /// ScratchBuffer - This class exposes a simple interface for the dynamic
        /// construction of tokens.  This is used for builtin macros (e.g. __LINE__) as
        /// well as token pasting, etc.
        class ScratchBuffer {
            SourceManager &SourceMgr;
            char *CurBuffer;
            SourceLocation BufferStartLoc;
            unsigned BytesUsed;

        public:
            ScratchBuffer(SourceManager &SM);

            /// getToken - Splat the specified text into a temporary MemoryBuffer and
            /// return a SourceLocation that refers to the token.  This is just like the
            /// previous method, but returns a location that indicates the physloc of the
            /// token.
            SourceLocation getToken(const char *Buf, unsigned Len, const char *&DestPtr);

        private:
            void AllocScratchBuffer(unsigned RequestLen);
        };
    }
}

#endif//CPTOYC_SCRATCHBUFFER_H
