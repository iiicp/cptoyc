/**********************************
* File:     FileManager.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/2
***********************************/

#ifndef CPTOYC_FILEMANAGER_H
#define CPTOYC_FILEMANAGER_H

#include "../llvm/StringMap.h"
#include "../llvm/OwningPtr.h"
#include "../llvm/Allocator.h"
#include <map>
#include <set>
#include <string>
// FIXME: Enhance libsystem to support inode and other fields in stat.
#include <sys/types.h>
#include <sys/stat.h>

namespace CPToyC {
    namespace Compiler {
        class FileManager;

/// DirectoryEntry - Cached information about one directory on the disk.
        ///
        class DirectoryEntry {
            const char  *Name;   // Name of the directory.
            friend class FileManager;
        public:
            DirectoryEntry() : Name(nullptr) {}
            const char *getName() const { return Name; }
        };

        class FileEntry {
            const char *Name;           // Name of the file.
            off_t Size;                 // File Size in bytes
            time_t ModTime;             // Modification time of file
            const DirectoryEntry *Dir;  // Directory file lives in.
            unsigned UID;               // A unique (small) ID for the file.
            dev_t Device;               // ID for the device containing the file.
            ino_t Inode;                // Inode number for the file.
            mode_t FileMode;            // The file mode as returned by 'stat'.
            friend class FileManager;
        public:
            FileEntry(dev_t device, ino_t inode, mode_t m)
                    : Name(nullptr), Device(device), Inode(inode), FileMode(m) {}
            // Add a default constructor for use with llvm::StringMap
            FileEntry() : Name(nullptr), Device(0), Inode(0), FileMode(0) {}

            const char *getName() const { return Name; }
            off_t getSize() const { return Size; }
            unsigned getUID() const { return UID; }
            ino_t getInode() const { return Inode; }
            dev_t getDevice() const { return Device; }
            time_t getModificationTime() const { return ModTime; }
            mode_t getFileMode() const { return FileMode; }

            /// getDir - Return the directory the file lives in.
            ///
            const DirectoryEntry *getDir() const { return Dir; }

            bool operator<(const FileEntry& RHS) const {
                return Device < RHS.Device || (Device == RHS.Device && Inode < RHS.Inode);
            }
        };

        // FIXME: This is a lightweight shim that is used by FileManager to cache
        //  'stat' system calls.  We will use it with PTH to identify if caching
        //  stat calls in PTH files is a performance win.
        class StatSysCallCache {
        public:
            virtual ~StatSysCallCache() {}
            virtual int stat(const char *path, struct stat *buf) = 0;
        };

        /// \brief A stat listener that can be used by FileManager to keep
        /// track of the results of stat() calls that occur throughout the
        /// execution of the front end.
        class MemorizeStatCalls : public StatSysCallCache {
        public:
            /// \brief The result of a stat() call.
            ///
            /// The first member is the result of calling stat(). If stat()
            /// found something, the second member is a copy of the stat
            /// structure.
            typedef std::pair<int, struct stat> StatResult;

            /// \brief The set of stat() calls that have been
            llvm::StringMap<StatResult, llvm::BumpPtrAllocator> StatCalls;

            typedef llvm::StringMap<StatResult, llvm::BumpPtrAllocator>::const_iterator
                    iterator;

            iterator begin() const { return StatCalls.begin(); }
            iterator end() const { return StatCalls.end(); }

            virtual int stat(const char *path, struct stat *buf);
        };


        class FileManager {

            class UniqueDirContainer;
            class UniqueFileContainer;

            /// UniqueDirs/UniqueFiles - Cache for existing directories/files.
            ///
            UniqueDirContainer &UniqueDirs;
            UniqueFileContainer &UniqueFiles;

            /// DirEntries/FileEntries - This is a cache of directory/file entries we have
            /// looked up.  The actual Entry is owned by UniqueFiles/UniqueDirs above.
            ///
            llvm::StringMap<DirectoryEntry*, llvm::BumpPtrAllocator> DirEntries;
            llvm::StringMap<FileEntry*, llvm::BumpPtrAllocator> FileEntries;


            /// NextFileUID - Each FileEntry we create is assigned a unique ID #.
            ///
            unsigned NextFileUID;

            // Statistics.
            unsigned NumDirLookups, NumFileLookups;
            unsigned NumDirCacheMisses, NumFileCacheMisses;

            // Caching.
            llvm::OwningPtr<StatSysCallCache> StatCache;

            int stat_cached(const char* path, struct stat* buf) {
                return StatCache.get() ? StatCache->stat(path, buf) : stat(path, buf);
            }
        public:
            FileManager();
            ~FileManager();

            /// setStatCache - Installs the provided StatSysCallCache object within
            ///  the FileManager.  Ownership of this object is transferred to the
            ///  FileManager.
            void setStatCache(StatSysCallCache *statCache) {
                StatCache.reset(statCache);
            }

            /// getDirectory - Lookup, cache, and verify the specified directory.  This
            /// returns null if the directory doesn't exist.
            ///
            const DirectoryEntry *getDirectory(const std::string &Filename) {
                return getDirectory(&Filename[0], &Filename[0] + Filename.size());
            }
            const DirectoryEntry *getDirectory(const char *FileStart,const char *FileEnd);

            /// getFile - Lookup, cache, and verify the specified file.  This returns null
            /// if the file doesn't exist.
            ///
            const FileEntry *getFile(const std::string &Filename) {
                return getFile(&Filename[0], &Filename[0] + Filename.size());
            }
            const FileEntry *getFile(const char *FilenameStart,
                                     const char *FilenameEnd);

            void PrintStats() const;
        };
    }
}


#endif//CPTOYC_FILEMANAGER_H
