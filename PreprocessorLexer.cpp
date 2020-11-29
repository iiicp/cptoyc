/**********************************
* File:     PreprocessorLexer.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/30
***********************************/

#include "PreprocessorLexer.h"
#include "Preprocessor.h"
#include "SourceManager.h"

using namespace CPToyC::Compiler;

/// LexIncludeFilename - After the preprocessor has parsed a #include, lex and
/// (potentially) macro expand the filename.
void PreprocessorLexer::LexIncludeFilename(Token &FilenameTok) {
    assert(ParsingPreprocessorDirective &&
                   ParsingFilename == false &&
                   "Must be in a preprocessing directive!");

    // We are now parsing a filename!
    ParsingFilename = true;

    // Lex the filename.
    IndirectLex(FilenameTok);

    // We should have obtained the filename now.
    ParsingFilename = false;

    // No filename?
//    if (FilenameTok.is(tok::eom))
//        PP->Diag(FilenameTok.getLocation(), diag::err_pp_expects_filename);
}

/// getFileEntry - Return the FileEntry corresponding to this FileID.  Like
/// getFileID(), this only works for lexers with attached preprocessors.
const FileEntry *PreprocessorLexer::getFileEntry() const {
    return PP->getSourceManager().getFileEntryForID(getFileID());
}
