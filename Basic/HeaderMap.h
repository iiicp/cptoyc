/**********************************
* File:     HeaderMap.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/1
***********************************/

#ifndef CPTOYC_HEADERMAP_H
#define CPTOYC_HEADERMAP_H
#include <cstdint>

namespace CPToyC {
    namespace Compiler {
        class FileEntry;
        class FileManager;
        class MemoryBuffer;

        struct HMapBucket {
            uint32_t Key;          // Offset (into strings) of key.

            uint32_t Prefix;     // Offset (into strings) of value prefix.
            uint32_t Suffix;     // Offset (into strings) of value suffix.
        };

        struct HMapHeader {
            uint32_t Magic;           // Magic word, also indicates byte order.
            uint16_t Version;         // Version number -- currently 1.
            uint16_t Reserved;        // Reserved for future use - zero for now.
            uint32_t StringsOffset;   // Offset to start of string pool.
            uint32_t NumEntries;      // Number of entries in the string table.
            uint32_t NumBuckets;      // Number of buckets (always a power of 2).
            uint32_t MaxValueLength;  // Length of longest result path (excluding nul).
            // An array of 'NumBuckets' HMapBucket objects follows this header.
            // Strings follow the buckets, at StringsOffset.
        };
        /// This class represents an Apple concept known as a 'header map'.  To the
        /// #include file resolution process, it basically acts like a directory of
        /// symlinks to files.  Its advantages are that it is dense and more efficient
        /// to create and process than a directory of symlinks.
        class HeaderMap {
            HeaderMap(const HeaderMap&); // DO NOT IMPLEMENT
            void operator=(const HeaderMap&); // DO NOT IMPLEMENT

            const MemoryBuffer *FileBuffer;
            bool NeedsBSwap;

            HeaderMap(const MemoryBuffer *File, bool BSwap)
                    : FileBuffer(File), NeedsBSwap(BSwap) {
            }
        public:
            ~HeaderMap();

            /// HeaderMap::Create - This attempts to load the specified file as a header
            /// map.  If it doesn't look like a HeaderMap, it gives up and returns null.
            static const HeaderMap *Create(const FileEntry *FE);

            /// LookupFile - Check to see if the specified relative filename is located in
            /// this HeaderMap.  If so, open it and return its FileEntry.
            const FileEntry *LookupFile(const char *FilenameStart,const char *FilenameEnd,
                                        FileManager &FM) const;

            /// getFileName - Return the filename of the headermap.
            const char *getFileName() const;

            /// dump - Print the contents of this headermap to stderr.
            void dump() const;

        private:
            unsigned getEndianAdjustedWord(unsigned X) const;
            const HMapHeader &getHeader() const;
            HMapBucket getBucket(unsigned BucketNo) const;
            const char *getString(unsigned StrTabIdx) const;
        };
    }
}

#endif//CPTOYC_HEADERMAP_H
