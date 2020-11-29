/**********************************
* File:     token.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#ifndef CPTOYC_TOKEN_H
#define CPTOYC_TOKEN_H

#include <string>
#include <iostream>
#include "TokenKinds.h"
#include "SourceLocation.h"
#include <cstdlib>

namespace CPToyC {
    namespace Compiler {

        class IdentifierInfo;

        /// Token - This structure provides full information about a lexed token.
        /// It is not intended to be space efficient, it is intended to return as much
        /// information as possible about each returned token.  This is expected to be
        /// compressed into a smaller form if memory footprint is important.
        ///
        /// The parser can create a special "annotation token" representing a stream of
        /// tokens that were parsed and semantically resolved, e.g.: "foo::MyClass<int>"
        /// can be represented by a single typename annotation token that carries
        /// information about the SourceRange of the tokens and the type object.
        class Token {
        private:
            /// The location of the token.
            SourceLocation LocLL;

            // Conceptually these next two fields could be in a union.  However, this
            // causes gcc 4.2 to pessimize LexTokenInternal, a very performance critical
            // routine. Keeping as separate members with casts until a more beautiful fix
            // presents itself.

            /// UintData - This holds either the length of the token text, when
            /// a normal token, or the end of the SourceRange when an annotation
            /// token.
            unsigned UintData;

            /// PtrData - This is a union of four different pointer types, which depends
            /// on what type of token this is:
            ///  Identifiers, keywords, etc:
            ///    This is an IdentifierInfo*, which contains the uniqued identifier
            ///    spelling.
            ///  Literals:  isLiteral() returns true.
            ///    This is a pointer to the start of the token in a text buffer, which
            ///    may be dirty (have trigraphs / escaped newlines).
            ///  Annotations (resolved type names, C++ scopes, etc): isAnnotation().
            ///    This is a pointer to sema-specific data for the annotation token.
            ///  Other:
            ///    This is null.
            void *PtrData;

            /// Kind - The actual flavor of token this is.
            ///
            unsigned KindLL : 8;  // DON'T make Kind a 'tok::TokenKind';
            // MSVC will treat it as a signed char and
            // TokenKinds > 127 won't be handled correctly.

            /// Flags - Bits we track about this token, members of the TokenFlags enum.
            unsigned Flags : 8;

        public:
            // Various flags set per token:
            enum TokenFlags {
                StartOfLine   = 0x01,  // At start of line or only after whitespace.
                LeadingSpace  = 0x02,  // Whitespace exists before this token.
                DisableExpand = 0x04,  // This identifier may never be macro expanded.
                NeedsCleaning = 0x08   // Contained an escaped newline or trigraph.
            };

            tok::TokenKind getKind() const { return (tok::TokenKind)KindLL; }
            void setKind(tok::TokenKind K) { KindLL = K; }

            /// is/isNot - Predicates to check if this token is a specific kind, as in
            /// "if (Tok.is(tok::l_brace)) {...}".
            bool is(tok::TokenKind K) const { return KindLL == (unsigned) K; }
            bool isNot(tok::TokenKind K) const { return KindLL != (unsigned) K; }

            /// isLiteral - Return true if this is a "literal", like a numeric
            /// constant, string, etc.
            bool isLiteral() const {
                return is(tok::numeric_constant) || is(tok::char_constant) ||
                       is(tok::string_literal) || is(tok::wide_string_literal) ||
                       is(tok::angle_string_literal);
            }

            bool isAnnotation() const {
                return is(tok::annot_typename);
            }

            /// getLocation - Return a source location identifier for the specified
            /// offset in the current file.
            SourceLocation getLocation() const { return LocLL; }
            unsigned getLength() const {
                assert(!isAnnotation() && "Annotation tokens have no length field");
                return UintData;
            }

            void setLocation(SourceLocation L) { LocLL = L; }
            void setLength(unsigned Len) {
                assert(!isAnnotation() && "Annotation tokens have no length field");
                UintData = Len;
            }

            SourceLocation getAnnotationEndLoc() const {
                assert(isAnnotation() && "Used AnnotEndLocID on non-annotation token");
                return SourceLocation::getFromRawEncoding(UintData);
            }
            void setAnnotationEndLoc(SourceLocation L) {
                assert(isAnnotation() && "Used AnnotEndLocID on non-annotation token");
                UintData = L.getRawEncoding();
            }

            /// getAnnotationRange - SourceRange of the group of tokens that this
            /// annotation token represents.
            SourceRange getAnnotationRange() const {
                return SourceRange(getLocation(), getAnnotationEndLoc());
            }
            void setAnnotationRange(SourceRange R) {
                setLocation(R.getBegin());
                setAnnotationEndLoc(R.getEnd());
            }

            const char *getName() const {
                return tok::getTokenName( (tok::TokenKind) KindLL);
            }

            /// startToken - Reset all flags to cleared.
            ///
            void startToken() {
                KindLL = tok::unknown;
                Flags = 0;
                PtrData = 0;
                LocLL = SourceLocation();
            }

            IdentifierInfo *getIdentifierInfo() const {
                assert(!isAnnotation() && "Used IdentInfo on annotation token!");
                if (isLiteral())
                    return nullptr;
                return (IdentifierInfo*) PtrData;
            }
            void setIdentifierInfo(IdentifierInfo *II) {
                PtrData = (void*) II;
            }

            /// getLiteralData - For a literal token (numeric constant, string, etc), this
            /// returns a pointer to the start of it in the text buffer if known, null
            /// otherwise.
            const char *getLiteralData() const {
                assert(isLiteral() && "Cannot get literal data of non-literal");
                return reinterpret_cast<const char*>(PtrData);
            }
            void setLiteralData(const char *Ptr) {
                assert(isLiteral() && "Cannot set literal data of non-literal");
                PtrData = (void*)Ptr;
            }

            void *getAnnotationValue() const {
                assert(isAnnotation() && "Used AnnotVal on non-annotation token");
                return PtrData;
            }
            void setAnnotationValue(void *val) {
                assert(isAnnotation() && "Used AnnotVal on non-annotation token");
                PtrData = val;
            }

            /// setFlag - Set the specified flag.
            void setFlag(TokenFlags Flag) {
                Flags |= Flag;
            }

            /// clearFlag - Unset the specified flag.
            void clearFlag(TokenFlags Flag) {
                Flags &= ~Flag;
            }

            /// getFlags - Return the internal represtation of the flags.
            ///  Only intended for low-level operations such as writing tokens to
            //   disk.
            unsigned getFlags() const {
                return Flags;
            }

            /// setFlagValue - Set a flag to either true or false.
            void setFlagValue(TokenFlags Flag, bool Val) {
                if (Val)
                    setFlag(Flag);
                else
                    clearFlag(Flag);
            }

            /// isAtStartOfLine - Return true if this token is at the start of a line.
            ///
            bool isAtStartOfLine() const { return (Flags & StartOfLine) ? true : false; }

            /// hasLeadingSpace - Return true if this token has whitespace before it.
            ///
            bool hasLeadingSpace() const { return (Flags & LeadingSpace) ? true : false; }

            /// isExpandDisabled - Return true if this identifier token should never
            /// be expanded in the future, due to C99 6.10.3.4p2.
            bool isExpandDisabled() const {
                return (Flags & DisableExpand) ? true : false;
            }

            /// needsCleaning - Return true if this token has trigraphs or escaped
            /// newlines in it.
            ///
            bool needsCleaning() const { return (Flags & NeedsCleaning) ? true : false; }
        };

        /// PPConditionalInfo - Information about the conditional stack (#if directives)
        /// currently active.
        struct PPConditionalInfo {
            /// IfLoc - Location where the conditional started.
            ///
            SourceLocation IfLoc;

            /// WasSkipping - True if this was contained in a skipping directive, e.g.
            /// in a "#if 0" block.
            bool WasSkipping;

            /// FoundNonSkip - True if we have emitted tokens already, and now we're in
            /// an #else block or something.  Only useful in Skipping blocks.
            bool FoundNonSkip;

            /// FoundElse - True if we've seen a #else in this block.  If so,
            /// #elif/#else directives are not allowed.
            bool FoundElse;
        };
    }
}
#endif //CPTOYC_TOKEN_H
