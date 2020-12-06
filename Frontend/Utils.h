/***********************************
* File:     Utils.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/6
***********************************/

#ifndef CPTOYC_UTILS_H
#define CPTOYC_UTILS_H

namespace llvm {
    class raw_ostream;
}

namespace CPToyC {
    namespace Compiler {
        class Preprocessor;
        /// DoPrintPreprocessedInput - Implement -E -dM mode.
        void DoPrintMacros(Preprocessor &PP, llvm::raw_ostream* OS);

        /// DoPrintPreprocessedInput - Implement -E mode.
        void DoPrintPreprocessedInput(Preprocessor &PP, llvm::raw_ostream* OS,
                                      bool EnableCommentOutput,
                                      bool EnableMacroCommentOutput,
                                      bool DisableLineMarkers,
                                      bool DumpDefines);
    }
}

#endif //CPTOYC_UTILS_H
