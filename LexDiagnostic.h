//===--- DiagnosticLex.h - Diagnostics for liblex ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_DIAGNOSTICLEX_H
#define LLVM_CLANG_DIAGNOSTICLEX_H

#include "Diagnostic.h"

namespace CPToyC {
    namespace Compiler {
        namespace diag {
            enum {
                // SFINAE meaning => Substitution failure is not an error
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE) ENUM,
#define LEXSTART
#include "DiagnosticLexKinds.inc"
#undef DIAG
                NUM_BUILTIN_LEX_DIAGNOSTICS
            };
        }// end namespace diag
    }// end namespace Compiler
}// end namespace CPToyC

#endif
