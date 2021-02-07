#pragma once

#include <stdio.h>
#include <string>
#include <map>

typedef std::map<std::string, int> ZipContentsMap;		// maps path to a zip content id

class ZipFile
{
    struct TZipDirHeader;
    struct TZipDirFileHeader;
    struct TZipLocalHeader;

    FILE* m_file;		                // Zip file
    char* m_pDirData;	                // Raw data buffer.
    int  m_nEntries;	                // Number of entries.
    const TZipDirFileHeader** m_papDir; // Pointers to the dir entries in pDirData.

public:
    ZipContentsMap m_ZipContentsMap;

    ZipFile() { m_nEntries = 0; m_file = NULL; m_pDirData = NULL; }
    virtual ~ZipFile() { End(); fclose(m_file); }

    bool Init(const std::wstring& resFileName);
    void End();

    int GetNumFiles()const { return m_nEntries; }
    std::string GetFilename(int i) const;
    int GetFileLen(int i) const;
    bool ReadFile(int i, void* buffer);

    // Added to show multi-threaded decompression
    bool ReadLargeFile(int i, void* buffer, void (*progressCallback)(int, bool&));

    int Find(const std::string& path) const;
};