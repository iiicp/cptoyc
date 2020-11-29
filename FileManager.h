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

#include <map>
#include <unordered_map>
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
            std::string Name;   // Name of the directory.
            friend class FileManager;
        public:
            DirectoryEntry() : Name("") {}
            const char *getName() const { return Name.c_str(); }
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
            FileEntry() : Name(0), Device(0), Inode(0), FileMode(0) {}

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

            bool operator()(const FileEntry* LHS, const FileEntry* RHS) const
            {
                return Device < RHS->Device || (Device == RHS->Device && Inode < RHS->Inode);
            }
        };

        class FileManager {
            /// DirEntries/FileEntries - This is a cache of directory/file entries we have
            /// looked up.  The actual Entry is owned by UniqueFiles/UniqueDirs above.
            ///
            std::unordered_map<std::string, DirectoryEntry*> DirEntries;
            std::unordered_map<std::string, FileEntry*> FileEntries;

            /// NextFileUID - Each FileEntry we create is assigned a unique ID #.
            ///
            unsigned NextFileUID;

            // Statistics.
            unsigned NumDirLookups, NumFileLookups;
            unsigned NumDirCacheMisses, NumFileCacheMisses;
        public:
            FileManager();
            ~FileManager();

            /// getDirectory - Lookup, cache, and verify the specified directory.  This
            /// returns null if the directory doesn't exist.
            ///
            const DirectoryEntry *getDirectory(const std::string &Dirname);
            const DirectoryEntry *getDirectory(const char *FileStart,const char *FileEnd) {
                return getDirectory(std::string(FileStart, FileEnd));
            }

            /// getFile - Lookup, cache, and verify the specified file.  This returns null
            /// if the file doesn't exist.
            ///
            const FileEntry *getFile(const std::string &Filename);
            const FileEntry *getFile(const char *FilenameStart,
                                     const char *FilenameEnd) {
                return getFile(std::string(FilenameStart, FilenameEnd));
            }

            void PrintStats() const;
        };
    }
}


#endif//CPTOYC_FILEMANAGER_H
