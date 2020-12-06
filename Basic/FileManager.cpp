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
#include "../llvm/SmallString.h"
#include <iostream>

using namespace CPToyC::Compiler;

// FIXME: Enhance libsystem to support inode and other fields.
#include <sys/stat.h>

#if defined(_MSC_VER)
#define S_ISDIR(s) (_S_IFDIR & s)
#endif

/// NON_EXISTENT_DIR - A special value distinct from null that is used to
/// represent a dir name that doesn't exist on the disk.
#define NON_EXISTENT_DIR reinterpret_cast<DirectoryEntry*>((intptr_t)-1)

//===----------------------------------------------------------------------===//
// FileManager Helper Classes
//===----------------------------------------------------------------------===//
#define IS_DIR_SEPARATOR_CHAR(x) ((x) == '/')

//===----------------------------------------------------------------------===//
// Common logic.
//===----------------------------------------------------------------------===//

class FileManager::UniqueDirContainer {
/// UniqueDirs - Cache from ID's to existing directories/files.
    ///
    std::map<std::pair<dev_t, ino_t>, DirectoryEntry> UniqueDirs;

public:
    DirectoryEntry &getDirectory(const char *Name, struct stat &StatBuf) {
        return UniqueDirs[std::make_pair(StatBuf.st_dev, StatBuf.st_ino)];
    }

    size_t size() { return UniqueDirs.size(); }
};

class FileManager::UniqueFileContainer {
    /// UniqueFiles - Cache from ID's to existing directories/files.
    ///
    std::set<FileEntry> UniqueFiles;

public:
    FileEntry &getFile(const char *Name, struct stat &StatBuf) {
        return
                const_cast<FileEntry&>(
                        *UniqueFiles.insert(FileEntry(StatBuf.st_dev,
                                                      StatBuf.st_ino,
                                                      StatBuf.st_mode)).first);
    }

    size_t size() { return UniqueFiles.size(); }
};

FileManager::FileManager()
        : UniqueDirs(*new UniqueDirContainer),
          UniqueFiles(*new UniqueFileContainer),
          DirEntries(64), FileEntries(64), NextFileUID(0) {
    NumDirLookups = NumFileLookups = 0;
    NumDirCacheMisses = NumFileCacheMisses = 0;
}

FileManager::~FileManager() {
    delete &UniqueDirs;
    delete &UniqueFiles;
}

/// getDirectory - Lookup, cache, and verify the specified directory.  This
/// returns null if the directory doesn't exist.
///
const DirectoryEntry* FileManager::getDirectory(const char *NameStart,
                                                const char *NameEnd){
    ++NumDirLookups;
    llvm::StringMapEntry<DirectoryEntry *> &NamedDirEnt =
            DirEntries.GetOrCreateValue(NameStart, NameEnd);

    // See if there is already an entry in the map.
    if (NamedDirEnt.getValue())
        return NamedDirEnt.getValue() == NON_EXISTENT_DIR
               ? 0 : NamedDirEnt.getValue();

    ++NumDirCacheMisses;

    // By default, initialize it to invalid.
    NamedDirEnt.setValue(NON_EXISTENT_DIR);

    // Get the null-terminated directory name as stored as the key of the
    // DirEntries map.
    const char *InterndDirName = NamedDirEnt.getKeyData();

    // Check to see if the directory exists.
    struct stat StatBuf;
    if (stat_cached(InterndDirName, &StatBuf) ||   // Error stat'ing.
        !S_ISDIR(StatBuf.st_mode))          // Not a directory?
        return 0;

    // It exists.  See if we have already opened a directory with the same inode.
    // This occurs when one dir is symlinked to another, for example.
    DirectoryEntry &UDE = UniqueDirs.getDirectory(InterndDirName, StatBuf);

    NamedDirEnt.setValue(&UDE);
    if (UDE.getName()) // Already have an entry with this inode, return it.
        return &UDE;

    // Otherwise, we don't have this directory yet, add it.  We use the string
    // key from the DirEntries map as the string.
    UDE.Name  = InterndDirName;
    return &UDE;
}

/// NON_EXISTENT_FILE - A special value distinct from null that is used to
/// represent a filename that doesn't exist on the disk.
#define NON_EXISTENT_FILE reinterpret_cast<FileEntry*>((intptr_t)-1)

/// getFile - Lookup, cache, and verify the specified file.  This returns null
/// if the file doesn't exist.
const FileEntry *FileManager::getFile(const char *NameStart,
                                      const char *NameEnd) {
    ++NumFileLookups;

    // See if there is already an entry in the map.
    llvm::StringMapEntry<FileEntry *> &NamedFileEnt =
            FileEntries.GetOrCreateValue(NameStart, NameEnd);

    // See if there is already an entry in the map.
    if (NamedFileEnt.getValue())
        return NamedFileEnt.getValue() == NON_EXISTENT_FILE
               ? 0 : NamedFileEnt.getValue();

    ++NumFileCacheMisses;

    // By default, initialize it to invalid.
    NamedFileEnt.setValue(NON_EXISTENT_FILE);

    // Figure out what directory it is in.   If the string contains a / in it,
    // strip off everything after it.
    // FIXME: this logic should be in sys::Path.
    const char *SlashPos = NameEnd-1;
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
        return 0;

    // Get the null-terminated file name as stored as the key of the
    // FileEntries map.
    const char *InterndFileName = NamedFileEnt.getKeyData();

    // FIXME: Use the directory info to prune this, before doing the stat syscall.
    // FIXME: This will reduce the # syscalls.

    // Nope, there isn't.  Check to see if the file exists.
    struct stat StatBuf;
    //llvm::cerr << "STATING: " << Filename;
    if (stat_cached(InterndFileName, &StatBuf) ||   // Error stat'ing.
        S_ISDIR(StatBuf.st_mode)) {           // A directory?
        // If this file doesn't exist, we leave a null in FileEntries for this path.
        //llvm::cerr << ": Not existing\n";
        return 0;
    }
    //llvm::cerr << ": exists\n";

    // It exists.  See if we have already opened a file with the same inode.
    // This occurs when one dir is symlinked to another, for example.
    FileEntry &UFE = UniqueFiles.getFile(InterndFileName, StatBuf);

    NamedFileEnt.setValue(&UFE);
    if (UFE.getName())  // Already have an entry with this inode, return it.
        return &UFE;

    // Otherwise, we don't have this directory yet, add it.
    // FIXME: Change the name to be a char* that points back to the 'FileEntries'
    // key.
    UFE.Name    = InterndFileName;
    UFE.Size    = StatBuf.st_size;
    UFE.ModTime = StatBuf.st_mtime;
    UFE.Dir     = DirInfo;
    UFE.UID     = NextFileUID++;
    return &UFE;
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

int MemorizeStatCalls::stat(const char *path, struct stat *buf) {
    int result = ::stat(path, buf);

    if (result != 0) {
        // Cache failed 'stat' results.
        struct stat empty;
        memset(&empty, 0, sizeof(empty));
        StatCalls[path] = StatResult(result, empty);
    }
    else if (!S_ISDIR(buf->st_mode) || path[0] == '/') {
        // Cache file 'stat' results and directories with absolutely
        // paths.
        StatCalls[path] = StatResult(result, *buf);
    }

    return result;
}