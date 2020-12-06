add_definitions("-std=c++17")
add_definitions("-Wall")

add_library(
        Lex

        STATIC

        lexer.cpp
        LiteralSupport.cpp
        MacroArgs.cpp
        MacroInfo.cpp
        PPCaching.cpp
        PPDirectives.cpp
        PPExpressions.cpp
        PPLexerChange.cpp
        PPMacroExpansion.cpp
        Preprocessor.cpp
        PreprocessorLexer.cpp
        ScratchBuffer.cpp
        TokenConcatenation.cpp
        TokenLexer.cpp
        )