#include <iostream>
#include "Preprocessor.h"
#include "FileManager.h"
#include "SourceManager.h"
#include <string>
#include <vector>
#include "HeaderSearch.h"

using namespace CPToyC::Compiler;

enum ProgActions {
    RewriteObjC,                  // ObjC->C Rewriter.
    RewriteBlocks,                // ObjC->C Rewriter for Blocks.
    RewriteMacros,                // Expand macros but not #includes.
    RewriteTest,                  // Rewriter playground
    FixIt,                        // Fix-It Rewriter
    HTMLTest,                     // HTML displayer testing stuff.
    EmitAssembly,                 // Emit a .s file.
    EmitLLVM,                     // Emit a .ll file.
    EmitBC,                       // Emit a .bc file.
    EmitLLVMOnly,                 // Generate LLVM IR, but do not
    EmitHTML,                     // Translate input source into HTML.
    ASTPrint,                     // Parse ASTs and print them.
    ASTPrintXML,                  // Parse ASTs and print them in XML.
    ASTDump,                      // Parse ASTs and dump them.
    ASTView,                      // Parse ASTs and view them in Graphviz.
    PrintDeclContext,             // Print DeclContext and their Decls.
    ParsePrintCallbacks,          // Parse and print each callback.
    ParseSyntaxOnly,              // Parse and perform semantic analysis.
    ParseNoop,                    // Parse with noop callbacks.
    RunPreprocessorOnly,          // Just lex, no output.
    PrintPreprocessedInput,       // -E mode.
    DumpTokens,                   // Dump out preprocessed tokens.
    DumpRawTokens,                // Dump out raw tokens.
    RunAnalysis,                  // Run one or more source code analyses.
    GeneratePTH,                  // Generate pre-tokenized header.
    GeneratePCH,                  // Generate pre-compiled header.
    InheritanceView               // View C++ inheritance for a specified class.
};

class DriverPreprocessorFactory : public PreprocessorFactory {
    SourceManager &SourceMgr;
    HeaderSearch &HeaderInfo;

public:
    DriverPreprocessorFactory(SourceManager &SM, HeaderSearch &Headers)
        : SourceMgr(SM), HeaderInfo(Headers) {
    }

    virtual ~DriverPreprocessorFactory() {}

    virtual Preprocessor * CreatePreprocessor() {
        return (new Preprocessor(SourceMgr, HeaderInfo));
    }
};

static void ProcessInputFile(Preprocessor &PP, PreprocessorFactory &PPF,
                             const std::string &InFile, ProgActions PA) {
    bool ClearSourceMgr = false;
    bool CompleteTranslationUnit = true;

    switch (PA) {
        case DumpRawTokens: {
            SourceManager &SM = PP.getSourceManager();
            // Start lexing the specified input file.
            Lexer RawLex(SM.getMainFileID(), SM);
            RawLex.SetKeepWhitespaceMode(true);

            Token RawTok;
            RawLex.LexFromRawLexer(RawTok);
            while (RawTok.isNot(tok::eof)) {
                PP.DumpToken(RawTok, true);
                fprintf(stderr, "\n");
                RawLex.LexFromRawLexer(RawTok);
            }
            ClearSourceMgr = true;
            break;
        }
        case DumpTokens: {
            Token Tok;
            // Start preprocessing the specified input file.
            PP.EnterMainSourceFile();
            do {
                PP.Lex(Tok);
                PP.DumpToken(Tok, true);
                fprintf(stderr, "\n");
            } while (Tok.isNot(tok::eof));
            ClearSourceMgr = true;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "./cptoyc filename" << std::endl;
		return 0;
	}
	std::unique_ptr<SourceManager> SourceMgr = nullptr;
	FileManager FileMgr;

    for (int i = 0; i < 1; ++i) {
        std::string InFile = argv[1];

        if (!SourceMgr) {
            SourceMgr.reset(new SourceManager());
        } else {
            SourceMgr->clearIDTables();
        }

        // Process the -I options and set them in the HeaderInfo.
        HeaderSearch HeaderInfo(FileMgr);

        // Set up the preprocessor with these options.
        DriverPreprocessorFactory PPFactory(*SourceMgr.get(), HeaderInfo);
        std::unique_ptr<Preprocessor> PP(PPFactory.CreatePreprocessor());
        const FileEntry *File = FileMgr.getFile(InFile);
        if (File) SourceMgr->createMainFileID(File, SourceLocation());
        if (SourceMgr->getMainFileID().isInvalid()) {
            std::cerr << "err_fe_error_reading" << std::endl;
            return 0;
        }
        ProcessInputFile(*PP, PPFactory, InFile, DumpTokens);
        HeaderInfo.ClearFileInfo();
    }
	return 0;
}