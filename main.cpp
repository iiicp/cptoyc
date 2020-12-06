#include <iostream>
#include <string>
#include "Lex/Preprocessor.h"
#include "Basic/FileManager.h"
#include "Basic/SourceManager.h"
#include "Basic/HeaderSearch.h"
#include "Frontend/TextDiagnosticBuffer.h"
#include "Frontend/InitHeaderSearch.h"
#include "Frontend/Utils.h"
#include "llvm/raw_ostream.h"

using namespace CPToyC::Compiler;

bool VerifyDiagnostics = true;

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
    Diagnostic        &Diags;
    const LangOptions &LangInfo;
    SourceManager &SourceMgr;
    HeaderSearch &HeaderInfo;

public:
    DriverPreprocessorFactory(Diagnostic &diags, const LangOptions &opts,
                              SourceManager &SM, HeaderSearch &Headers)
        : Diags(diags), LangInfo(opts), SourceMgr(SM), HeaderInfo(Headers) {
    }

    virtual ~DriverPreprocessorFactory() {}

    virtual Preprocessor * CreatePreprocessor() {
        // Create the Preprocessor.
        llvm::OwningPtr<Preprocessor> PP(new Preprocessor(Diags, LangInfo,
                                                          SourceMgr, HeaderInfo));
        return PP.take();
    }
};

/// InitializeIncludePaths - Process the -I options and set them in the
/// HeaderSearch object.
void InitializeIncludePaths(const char *Argv0, HeaderSearch &Headers,
                            FileManager &FM, const LangOptions &Lang) {
    InitHeaderSearch Init(Headers, false);
#if 0
    // Handle -I... and -F... options, walking the lists in parallel.
    unsigned Iidx = 0, Fidx = 0;
    while (Iidx < I_dirs.size() && Fidx < F_dirs.size()) {
        if (I_dirs.getPosition(Iidx) < F_dirs.getPosition(Fidx)) {
            Init.AddPath(I_dirs[Iidx], InitHeaderSearch::Angled, false, true, false);
            ++Iidx;
        } else {
            Init.AddPath(F_dirs[Fidx], InitHeaderSearch::Angled, false, true, true);
            ++Fidx;
        }
    }

    // Consume what's left from whatever list was longer.
    for (; Iidx != I_dirs.size(); ++Iidx)
        Init.AddPath(I_dirs[Iidx], InitHeaderSearch::Angled, false, true, false);
    for (; Fidx != F_dirs.size(); ++Fidx)
        Init.AddPath(F_dirs[Fidx], InitHeaderSearch::Angled, false, true, true);

    // Handle -idirafter... options.
    for (unsigned i = 0, e = idirafter_dirs.size(); i != e; ++i)
        Init.AddPath(idirafter_dirs[i], InitHeaderSearch::After,
                     false, true, false);

    // Handle -iquote... options.
    for (unsigned i = 0, e = iquote_dirs.size(); i != e; ++i)
        Init.AddPath(iquote_dirs[i], InitHeaderSearch::Quoted, false, true, false);

    // Handle -isystem... options.
    for (unsigned i = 0, e = isystem_dirs.size(); i != e; ++i)
        Init.AddPath(isystem_dirs[i], InitHeaderSearch::System, false, true, false);

    // Walk the -iprefix/-iwithprefix/-iwithprefixbefore argument lists in
    // parallel, processing the values in order of occurance to get the right
    // prefixes.
    {
        std::string Prefix = "";  // FIXME: this isn't the correct default prefix.
        unsigned iprefix_idx = 0;
        unsigned iwithprefix_idx = 0;
        unsigned iwithprefixbefore_idx = 0;
        bool iprefix_done           = iprefix_vals.empty();
        bool iwithprefix_done       = iwithprefix_vals.empty();
        bool iwithprefixbefore_done = iwithprefixbefore_vals.empty();
        while (!iprefix_done || !iwithprefix_done || !iwithprefixbefore_done) {
            if (!iprefix_done &&
                (iwithprefix_done ||
                 iprefix_vals.getPosition(iprefix_idx) <
                 iwithprefix_vals.getPosition(iwithprefix_idx)) &&
                (iwithprefixbefore_done ||
                 iprefix_vals.getPosition(iprefix_idx) <
                 iwithprefixbefore_vals.getPosition(iwithprefixbefore_idx))) {
                Prefix = iprefix_vals[iprefix_idx];
                ++iprefix_idx;
                iprefix_done = iprefix_idx == iprefix_vals.size();
            } else if (!iwithprefix_done &&
                       (iwithprefixbefore_done ||
                        iwithprefix_vals.getPosition(iwithprefix_idx) <
                        iwithprefixbefore_vals.getPosition(iwithprefixbefore_idx))) {
                Init.AddPath(Prefix+iwithprefix_vals[iwithprefix_idx],
                             InitHeaderSearch::System, false, false, false);
                ++iwithprefix_idx;
                iwithprefix_done = iwithprefix_idx == iwithprefix_vals.size();
            } else {
                Init.AddPath(Prefix+iwithprefixbefore_vals[iwithprefixbefore_idx],
                             InitHeaderSearch::Angled, false, false, false);
                ++iwithprefixbefore_idx;
                iwithprefixbefore_done =
                        iwithprefixbefore_idx == iwithprefixbefore_vals.size();
            }
        }
    }

    Init.AddDefaultEnvVarPaths(Lang);

    // Add the clang headers, which are relative to the clang binary.
    llvm::sys::Path MainExecutablePath =
            llvm::sys::Path::GetMainExecutable(Argv0,
                                               (void*)(intptr_t)InitializeIncludePaths);
    if (!MainExecutablePath.isEmpty()) {
        MainExecutablePath.eraseComponent();  // Remove /clang from foo/bin/clang
        MainExecutablePath.eraseComponent();  // Remove /bin   from foo/bin

        // Get foo/lib/clang/<version>/include
        MainExecutablePath.appendComponent("lib");
        MainExecutablePath.appendComponent("clang");
        MainExecutablePath.appendComponent(CLANG_VERSION_STRING);
        MainExecutablePath.appendComponent("include");

        // We pass true to ignore sysroot so that we *always* look for clang headers
        // relative to our executable, never relative to -isysroot.
        Init.AddPath(MainExecutablePath.c_str(), InitHeaderSearch::System,
                     false, false, false, true /*ignore sysroot*/);
    }

#endif
//    if (!nostdinc)
    Init.AddDefaultSystemIncludePaths(Lang);

    // Now that we have collected all of the include paths, merge them all
    // together and tell the preprocessor about them.

    Init.Realize();
}

static llvm::raw_ostream* ComputeOutFile(const std::string& InFile,
                                         const char* Extension,
                                         bool Binary) {
    llvm::raw_ostream* Ret;
//    bool UseStdout = false;
//    std::string OutFile;
//    if (OutputFile == "-" || (OutputFile.empty() && InFile == "-")) {
//        UseStdout = true;
//    } else if (!OutputFile.empty()) {
//        OutFile = OutputFile;
//    } else if (Extension) {
//        llvm::sys::Path Path(InFile);
//        Path.eraseSuffix();
//        Path.appendSuffix(Extension);
//        OutFile = Path.toString();
//    } else {
//        UseStdout = true;
//    }

//    if (UseStdout) {
//        Ret = new llvm::raw_stdout_ostream();
//        if (Binary)
//            llvm::sys::Program::ChangeStdoutToBinary();
//    } else {
//        std::string Error;
//        Ret = new llvm::raw_fd_ostream(OutFile.c_str(), Binary,
//                /*Force=*/true, Error);
//        if (!Error.empty()) {
//            // FIXME: Don't fail this way.
//            llvm::cerr << "ERROR: " << Error << "\n";
//            ::exit(1);
//        }
//        OutPath = OutFile;
//    }
    Ret = new llvm::raw_stdout_ostream();

    return Ret;
}

static void ProcessInputFile(Preprocessor &PP, PreprocessorFactory &PPF,
                             const std::string &InFile, ProgActions PA) {
    llvm::OwningPtr<llvm::raw_ostream> OS;
    bool ClearSourceMgr = false;
    bool CompleteTranslationUnit = true;

    switch (PA) {
        case DumpRawTokens: {
            SourceManager &SM = PP.getSourceManager();
            // Start lexing the specified input file.
            Lexer RawLex(SM.getMainFileID(), SM, PP.getLangOptions());
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
        case PrintPreprocessedInput:
            OS.reset(ComputeOutFile(InFile, nullptr, true));
            break;
        case ParseNoop:
            break;
        case RunPreprocessorOnly:
            break;
    }

    if (PA == RunPreprocessorOnly) {    // Just lex as fast as we can, no output.
        Token Tok;
        // Start parsing the specified input file.
        PP.EnterMainSourceFile();
        do {
            PP.Lex(Tok);
        } while (Tok.isNot(tok::eof));
        ClearSourceMgr = true;
    } else if (PA == ParseNoop) {                  // -parse-noop
#if 0
        ParseFile(PP, new MinimalAction(PP));
        ClearSourceMgr = true;
#endif
    } else if (PA == PrintPreprocessedInput){  // -E mode.
        if (false)
            DoPrintMacros(PP, OS.get());
        else
            DoPrintPreprocessedInput(PP, OS.get(), false,
                                     false,
                                     true, false);
        ClearSourceMgr = true;
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "./cptoyc filename" << std::endl;
		return 0;
	}



	llvm::OwningPtr<DiagnosticClient> DiagClient;
	if (VerifyDiagnostics) {
	    DiagClient.reset(new TextDiagnosticBuffer());
	}

    // Configure our handling of diagnostics.
    Diagnostic Diags(DiagClient.get());

    llvm::OwningPtr<SourceManager> SourceMgr;

    // Create a file manager object to provide access to and cache the filesystem.
	FileManager FileMgr;

    for (int i = 0; i < 1; ++i) {
        std::string InFile = argv[1];

        if (!SourceMgr) {
            SourceMgr.reset(new SourceManager());
        } else {
            SourceMgr->clearIDTables();
        }

        // Initialize language options, inferring file types from input filenames.
        LangOptions LangInfo;
        DiagClient->setLangOptions(&LangInfo);
        LangInfo.Bool = 1;
        LangInfo.BCPLComment = 1;
        LangInfo.C99 = 1;
        LangInfo.HexFloats = 1;
        LangInfo.CharIsSigned = 1;
        LangInfo.ImplicitInt = 1;

        // Process the -I options and set them in the HeaderInfo.
        HeaderSearch HeaderInfo(FileMgr);

        InitializeIncludePaths(argv[0], HeaderInfo, FileMgr, LangInfo);

        // Set up the preprocessor with these options.
        DriverPreprocessorFactory PPFactory(Diags, LangInfo, *SourceMgr.get(), HeaderInfo);

        llvm::OwningPtr<Preprocessor> PP(PPFactory.CreatePreprocessor());

        const FileEntry *File = FileMgr.getFile(InFile);
        if (File) SourceMgr->createMainFileID(File, SourceLocation());
        if (SourceMgr->getMainFileID().isInvalid()) {
//            PP->getDiagnostics().Report(FullSourceLoc(), diag::err_fe_error_reading)
//                    << InFile.c_str();
            std::cout << "err_fe_error_reading" << std::endl;
            return 0;
        }
        ProcessInputFile(*PP, PPFactory, InFile, PrintPreprocessedInput);
        HeaderInfo.ClearFileInfo();
    }
	return 0;
}