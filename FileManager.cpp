/**********************************
* File:     FileManager.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/2
***********************************/

#include "FileManager.h"
#include <iostream>

using namespace CPToyC::Compiler;

// FIXME: Enhance libsystem to support inode and other fields.
#include <sys/stat.h>

#if defined(_MSC_VER)
#define S_ISDIR(s) (_S_IFDIR & s)
#endif

//===----------------------------------------------------------------------===//
// FileManager Helper Classes
//===----------------------------------------------------------------------===//
#define IS_DIR_SEPARATOR_CHAR(x) ((x) == '/')

/// NON_EXISTENT_DIR - A special value distinct from null that is used to
/// represent a dir name that doesn't exist on the disk.
#define NON_EXISTENT_DIR reinterpret_cast<DirectoryEntry*>((intptr_t)-1)

//===----------------------------------------------------------------------===//
// Common logic.
//===----------------------------------------------------------------------===//

FileManager::FileManager()
        : NextFileUID(0) {
    NumDirLookups = NumFileLookups = 0;
    NumDirCacheMisses = NumFileCacheMisses = 0;
}

FileManager::~FileManager() {

    if (FileEntries.size()) {
        for (auto v : FileEntries) {
            delete v.second;
        }
    }

    if (DirEntries.size()) {
        for (auto v : DirEntries) {
            delete v.second;
        }
    }
}

/// getDirectory - Lookup, cache, and verify the specified directory.  This
/// returns null if the directory doesn't exist.
///
const DirectoryEntry* FileManager::getDirectory(const std::string &DirName){
    ++NumDirLookups;

    if (DirEntries.find(DirName) != DirEntries.end()) {
        return DirEntries[DirName];
    }

    ++NumDirCacheMisses;


    // Get the null-terminated directory name as stored as the key of the
    // DirEntries map.
    const char *InterndDirName = DirName.c_str();

    // Check to see if the directory exists.
    struct stat StatBuf;
    if (stat(InterndDirName, &StatBuf) ||   // Error stat'ing.
        !S_ISDIR(StatBuf.st_mode))          // Not a directory?
        return nullptr;

    // It exists.  See if we have already opened a directory with the same inode.
    // This occurs when one dir is symlinked to another, for example.
    DirectoryEntry *UDE = new DirectoryEntry();
    UDE->Name = DirName;
    DirEntries[DirName] = UDE;
    return UDE;
}

/// NON_EXISTENT_FILE - A special value distinct from null that is used to
/// represent a filename that doesn't exist on the disk.
#define NON_EXISTENT_FILE reinterpret_cast<FileEntry*>((intptr_t)-1)

/// getFile - Lookup, cache, and verify the specified file.  This returns null
/// if the file doesn't exist.
const FileEntry *FileManager::getFile(const std::string &Filename) {
    ++NumFileLookups;

    if (FileEntries.find(Filename) != FileEntries.end()) {
        return FileEntries[Filename];
    }

    ++NumFileCacheMisses;

    const char *NameEnd = Filename.c_str() + Filename.size();
    const char *NameStart = Filename.c_str();

    // Figure out what directory it is in.   If the string contains a / in it,
    // strip off everything after it.
    // FIXME: this logic should be in sys::Path.
    const char *SlashPos = NameEnd - 1;
    while (SlashPos >= NameStart && !IS_DIR_SEPARATOR_CHAR(SlashPos[0]))
        --SlashPos;
    // Ignore duplicate //'s.
    while (SlashPos > NameStart && IS_DIR_SEPARATOR_CHAR(SlashPos[-1]))
        --SlashPos;

    const DirectoryEntry *DirInfo;
    if (SlashPos < NameStart) {
        // Use the current directory if file has no path component.
        const char *Name = ".";
        DirInfo = getDirectory(Name, Name+1);
    } else if (SlashPos == NameEnd-1)
        return 0;       // If filename ends with a /, it's a directory.
    else
        DirInfo = getDirectory(NameStart, SlashPos);

    if (DirInfo == 0)  // Directory doesn't exist, file can't exist.
        return nullptr;

    // Get the null-terminated file name as stored as the key of the
    // FileEntries map.
    const char *InterndFileName = Filename.c_str();

    // FIXME: Use the directory info to prune this, before doing the stat syscall.
    // FIXME: This will reduce the # syscalls.

    // Nope, there isn't.  Check to see if the file exists.
    struct stat StatBuf;
    //llvm::cerr << "STATING: " << Filename;
    if (stat(InterndFileName, &StatBuf) ||   // Error stat'ing.
        S_ISDIR(StatBuf.st_mode)) {           // A directory?
        // If this file doesn't exist, we leave a null in FileEntries for this path.
        std::cerr << ": Not existing\n";
        return nullptr;
    }
    //llvm::cerr << ": exists\n";

    // It exists.  See if we have already opened a file with the same inode.
    // This occurs when one dir is symlinked to another, for example.
    FileEntry *UFE = new FileEntry();

    FileEntries[Filename] = UFE;
    // Otherwise, we don't have this directory yet, add it.
    // FIXME: Change the name to be a char* that points back to the 'FileEntries'
    // key.
    UFE->Name    = InterndFileName;
    UFE->Size    = StatBuf.st_size;
    UFE->ModTime = StatBuf.st_mtime;
    UFE->Dir     = DirInfo;
    UFE->UID     = NextFileUID++;
    return UFE;
}

void FileManager::PrintStats() const {
    std::cerr << "\n*** File Manager Stats:\n";
    std::cerr << DirEntries.size() << " files found, "
              << FileEntries.size() << " dirs found.\n";
    std::cerr << NumDirLookups << " dir lookups, "
              << NumDirCacheMisses << " dir cache misses.\n";
    std::cerr << NumFileLookups << " file lookups, "
              << NumFileCacheMisses << " file cache misses.\n";

    //llvm::cerr << PagesMapped << BytesOfPagesMapped << FSLookups;
}