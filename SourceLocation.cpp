/**********************************
* File:     SourceLocation.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/2
***********************************/

#include "SourceLocation.h"
#include "SourceManager.h"
#include <iostream>

using namespace CPToyC::Compiler;

//===----------------------------------------------------------------------===//
// SourceLocation
//===----------------------------------------------------------------------===//

void SourceLocation::print(std::ostream &OS, const SourceManager &SM)const{
    if (!isValid()) {
        OS << "<invalid loc>";
        return;
    }

    if (isFileID()) {
        PresumedLoc PLoc = SM.getPresumedLoc(*this);
        // The instantiation and spelling pos is identical for file locs.
        OS << PLoc.getFilename() << ':' << PLoc.getLine()
           << ':' << PLoc.getColumn();
        return;
    }

    SM.getInstantiationLoc(*this).print(OS, SM);

    OS << " <Spelling=";
    SM.getSpellingLoc(*this).print(OS, SM);
    OS << '>';
}

void SourceLocation::dump(const SourceManager &SM) const {
    print(std::cerr, SM);
}

//===----------------------------------------------------------------------===//
// FullSourceLoc
//===----------------------------------------------------------------------===//

FileID FullSourceLoc::getFileID() const {
    assert(isValid());
    return SrcMgr->getFileID(*this);
}


FullSourceLoc FullSourceLoc::getInstantiationLoc() const {
    assert(isValid());
    return FullSourceLoc(SrcMgr->getInstantiationLoc(*this), *SrcMgr);
}

FullSourceLoc FullSourceLoc::getSpellingLoc() const {
    assert(isValid());
    return FullSourceLoc(SrcMgr->getSpellingLoc(*this), *SrcMgr);
}

unsigned FullSourceLoc::getInstantiationLineNumber() const {
    assert(isValid());
    return SrcMgr->getInstantiationLineNumber(*this);
}

unsigned FullSourceLoc::getInstantiationColumnNumber() const {
    assert(isValid());
    return SrcMgr->getInstantiationColumnNumber(*this);
}

unsigned FullSourceLoc::getSpellingLineNumber() const {
    assert(isValid());
    return SrcMgr->getSpellingLineNumber(*this);
}

unsigned FullSourceLoc::getSpellingColumnNumber() const {
    assert(isValid());
    return SrcMgr->getSpellingColumnNumber(*this);
}

bool FullSourceLoc::isInSystemHeader() const {
    assert(isValid());
    return SrcMgr->isInSystemHeader(*this);
}

const char *FullSourceLoc::getCharacterData() const {
    assert(isValid());
    return SrcMgr->getCharacterData(*this);
}

const MemoryBuffer* FullSourceLoc::getBuffer() const {
    assert(isValid());
    return SrcMgr->getBuffer(SrcMgr->getFileID(*this));
}

std::pair<const char*, const char*> FullSourceLoc::getBufferData() const {
    const MemoryBuffer *Buf = getBuffer();
    return std::make_pair(Buf->getBufferStart(), Buf->getBufferEnd());
}

std::pair<FileID, unsigned> FullSourceLoc::getDecomposedLoc() const {
    return SrcMgr->getDecomposedLoc(*this);
}

