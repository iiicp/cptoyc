add_definitions("-std=c++17")
#add_definitions("-Wall")

add_library(
        Basic

        STATIC

        Diagnostic.cpp
        FileManager.cpp
        FileManager.cpp
        HeaderMap.cpp
        HeaderSearch.cpp
        IdentifierTable.cpp
        SourceLocation.cpp
        SourceLocation.cpp
        TokenKinds.cpp
#        DiagnosticAnalysisKinds.inc
#        DiagnosticLexKinds.inc
#        DiagnosticASTKinds.inc
#        DiagnosticCommonKinds.inc
#        DiagnosticDriverKinds.inc
#        DiagnosticFrontendKinds.inc
#        DiagnosticGroups.inc
#        DiagnosticParseKinds.inc
#        DiagnosticSemaKinds.inc
        )