/**********************************
* File:     IdentifierTable.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/1
***********************************/

#ifndef CPTOYC_IDENTIFIERTABLE_H
#define CPTOYC_IDENTIFIERTABLE_H

#include <unordered_map>
#include <string>
#include <cassert>
#include "TokenKinds.h"

namespace CPToyC {
    namespace Compiler {
        class IdentifierInfo {
            unsigned TokenID            : 8;        // tok::identifier
            bool HasMacro               : 1;        // True if there is a #define for this.
            bool NeedsHandleIdentifier  : 1;        // See "RecomputeNeedsHandleIdentifier".
            void *FETokenInfo;                      // language front-end

            std::unordered_map<std::string, IdentifierInfo *>::iterator Entry;
            std::unordered_map<std::string, IdentifierInfo *>::iterator End;

            IdentifierInfo(const IdentifierInfo&) = delete;  // NONCOPYABLE.
            void operator=(const IdentifierInfo&) = delete;  // NONASSIGNABLE.

            friend class IdentifierTable;

        public:
            IdentifierInfo();

            /// isStr - Return true if this is the identifier for the specified string.
            /// This is intended to be used for string literals only: II->isStr("foo").
            template <std::size_t StrLen>
            bool isStr(const char (&Str)[StrLen]) const {
                return getLength() == StrLen-1 && !memcmp(getName(), Str, StrLen-1);
            }

            /// getName - Return the actual string for this identifier.  The returned
            /// string is properly null terminated.
            ///
            const char *getName() const {
                if (Entry != End) return Entry->first.c_str();
                // FIXME: This is gross. It would be best not to embed specific details
                // of the PTH file format here.
                // The 'this' pointer really points to a
                // std::pair<IdentifierInfo, const char*>, where internal pointer
                // points to the external string data.
                return ((std::pair<IdentifierInfo, const char*>*) this)->second;
            }

            /// getLength - Efficiently return the length of this identifier info.
            ///
            unsigned getLength() const {
                if (Entry != End) return Entry->first.size();
                // FIXME: This is gross. It would be best not to embed specific details
                // of the PTH file format here.
                // The 'this' pointer really points to a
                // std::pair<IdentifierInfo, const char*>, where internal pointer
                // points to the external string data.
                const char* p = ((std::pair<IdentifierInfo, const char*>*) this)->second-2;
                return (((unsigned) p[0])
                        | (((unsigned) p[1]) << 8)) - 1;
            }

            /// hasMacroDefinition - Return true if this identifier is #defined to some
            /// other value.
            bool hasMacroDefinition() const {
                return HasMacro;
            }
            void setHasMacroDefinition(bool Val) {
                if (HasMacro == Val) return;

                HasMacro = Val;
                if (Val)
                    NeedsHandleIdentifier = 1;
                else
                    RecomputeNeedsHandleIdentifier();
            }

            /// get/setTokenID - If this is a source-language token (e.g. 'for'), this API
            /// can be used to cause the lexer to map identifiers to source-language
            /// tokens.
            tok::TokenKind getTokenID() const { return (tok::TokenKind)TokenID; }
            void setTokenID(tok::TokenKind ID) { TokenID = ID; }

            /// getPPKeywordID - Return the preprocessor keyword ID for this identifier.
            /// For example, "define" will return tok::pp_define.
            tok::PPKeywordKind getPPKeywordID() const;

            /// getFETokenInfo/setFETokenInfo - The language front-end is allowed to
            /// associate arbitrary metadata with this token.
            template<typename T>
            T *getFETokenInfo() const { return static_cast<T*>(FETokenInfo); }
            void setFETokenInfo(void *T) { FETokenInfo = T; }

            /// isHandleIdentifierCase - Return true if the Preprocessor::HandleIdentifier
            /// must be called on a token of this identifier.  If this returns false, we
            /// know that HandleIdentifier will not affect the token.
            bool isHandleIdentifierCase() const { return NeedsHandleIdentifier; }

            bool operator()(IdentifierInfo *LHS, IdentifierInfo *RHS) {
                return LHS->getLength() < RHS->getLength();
            }

        private:
            /// RecomputeNeedsHandleIdentifier - The Preprocessor::HandleIdentifier does
            /// several special (but rare) things to identifiers of various sorts.  For
            /// example, it changes the "for" keyword token from tok::identifier to
            /// tok::for.
            ///
            /// This method is very tied to the definition of HandleIdentifier.  Any
            /// change to it should be reflected here.
            void RecomputeNeedsHandleIdentifier() {
                NeedsHandleIdentifier = hasMacroDefinition();
            }
        };

        /// IdentifierInfoLookup - An abstract class used by IdentifierTable that
        ///  provides an interface for performing lookups from strings
        /// (const char *) to IdentiferInfo objects.
        class IdentifierInfoLookup {
        public:
            virtual ~IdentifierInfoLookup();

            /// get - Return the identifier token info for the specified named identifier.
            ///  Unlike the version in IdentifierTable, this returns a pointer instead
            ///  of a reference.  If the pointer is NULL then the IdentifierInfo cannot
            ///  be found.
            virtual IdentifierInfo* get(const char *NameStart, const char *NameEnd) = 0;
        };

        /// \brief An abstract class used to resolve numerical identifier
        /// references (meaningful only to some external source) into
        /// IdentifierInfo pointers.
        class ExternalIdentifierLookup {
        public:
            virtual ~ExternalIdentifierLookup();

            /// \brief Return the identifier associated with the given ID number.
            ///
            /// The ID 0 is associated with the NULL identifier.
            virtual IdentifierInfo *GetIdentifier(unsigned ID) = 0;
        };

        class IdentifierTable {
            std::unordered_map<std::string, IdentifierInfo *> HashTable;

            IdentifierInfoLookup* ExternalLookup;
        public:
            IdentifierTable(IdentifierInfoLookup *externalLookup = nullptr);

            /// \brief Set the external identifier lookup mechanism.
            void setExternalIdentifierLookup(IdentifierInfoLookup *IILookup) {
                ExternalLookup = IILookup;
            }

            /// get - Return the identifier token info for the specified named identifier.
            ///
            IdentifierInfo &get(const char *NameStart, const char *NameEnd) {

                std::string name(NameStart, NameEnd);

                IdentifierInfo *II;
                if (HashTable.find(name) != HashTable.end()) {
                    II = HashTable[name];
                    return *II;
                }

                // No entry; if we have an external lookup, look there first.
                if (ExternalLookup) {
                    II = ExternalLookup->get(NameStart, NameEnd);
                    if (II) {
                        // Cache in the StringMap for subsequent lookups.
                        HashTable[name] = II;
                        return *II;
                    }
                }

                std::pair<std::string, IdentifierInfo *> entry;
                entry.first = name;

                // Lookups failed, make a new IdentifierInfo.
                II = new IdentifierInfo();
                entry.second = II;

                HashTable.insert(entry);

                // Make sure getName() knows how to find the IdentifierInfo
                // contents.
                II->Entry = HashTable.find(name);
                II->End = HashTable.end();

                return *II;
            }

            /// \brief Creates a new IdentifierInfo from the given string.
            ///
            /// This is a lower-level version of get() that requires that this
            /// identifier not be known previously and that does not consult an
            /// external source for identifiers. In particular, external
            /// identifier sources can use this routine to build IdentifierInfo
            /// nodes and then introduce additional information about those
            /// identifiers.
            IdentifierInfo &CreateIdentifierInfo(const char *NameStart,
                                                 const char *NameEnd) {
                std::string name(NameStart, NameEnd);

                IdentifierInfo *II;
                if (HashTable.find(name) != HashTable.end()) {
                    II = HashTable[name];
                    return *II;
                }

                // Lookups failed, make a new IdentifierInfo.
                II = new IdentifierInfo();
                HashTable[name] = II;

                // Make sure getName() knows how to find the IdentifierInfo
                // contents.
                II->Entry = HashTable.find(name);
                II->End = HashTable.end();

                return *II;
            }

            IdentifierInfo &get(const char *Name) {
                return get(Name, Name+strlen(Name));
            }
            IdentifierInfo &get(const std::string &Name) {
                // Don't use c_str() here: no need to be null terminated.
                const char *NameBytes = Name.data();
                return get(NameBytes, NameBytes+Name.size());
            }


            unsigned size() const { return HashTable.size(); }

            /// PrintStats - Print some statistics to stderr that indicate how well the
            /// hashing is doing.
            void AddKeywords();
        };
    }
}

#endif//CPTOYC_IDENTIFIERTABLE_H
