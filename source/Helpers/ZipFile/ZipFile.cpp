#include <zlib.h> // C:\tools\vcpkg\vcpkg.exe install zlib:x64-windows

#include <types.h>
#include <Helpers/ZipFile/ZipFile.h>


// --------------------------------------------------------------------------
// ZIP file structures. Note these have to be packed.
// Anytime you define a structure that will be stored onto a disk or in a stream, you should pack them.
// --------------------------------------------------------------------------

#pragma pack(1)
// --------------------------------------------------------------------------
// struct ZipFile::TZipLocalHeader		
// --------------------------------------------------------------------------
struct ZipFile::TZipLocalHeader
{
	enum
	{
		SIGNATURE = 0x04034b50,
	};
	u32   sig;
	u16   version;
	u16    flag;
	u16    compression;      // Z_NO_COMPRESSION or Z_DEFLATED
	u16    modTime;
	u16    modDate;
	u32   crc32;
	u32   cSize;
	u32   ucSize;
	u16    fnameLen;         // Filename string follows header.
	u16    xtraLen;          // Extra field follows filename.
};

// --------------------------------------------------------------------------
// struct ZipFile::TZipDirHeader		
// --------------------------------------------------------------------------
struct ZipFile::TZipDirHeader
{
	enum
	{
		SIGNATURE = 0x06054b50
	};
	u32   sig;
	u16    nDisk;
	u16    nStartDisk;
	u16    nDirEntries;
	u16    totalDirEntries;
	u32   dirSize;
	u32   dirOffset;
	u16    cmntLen;
};

// --------------------------------------------------------------------------
// struct ZipFile::TZipDirFileHeader			
// --------------------------------------------------------------------------
struct ZipFile::TZipDirFileHeader
{
	enum
	{
		SIGNATURE = 0x02014b50
	};
	u32   sig;
	u16    verMade;
	u16    verNeeded;
	u16    flag;
	u16    compression;      // COMP_xxxx
	u16    modTime;
	u16    modDate;
	u32   crc32;
	u32   cSize;            // Compressed size
	u32   ucSize;           // Uncompressed size
	u16    fnameLen;         // Filename string follows header.
	u16    xtraLen;          // Extra field follows filename.
	u16    cmntLen;          // Comment field follows extra field.
	u16    diskStart;
	u16    intAttr;
	u32   extAttr;
	u32   hdrOffset;

	char* GetName() const { return (char*)(this + 1); }
	char* GetExtra() const { return GetName() + fnameLen; }
	char* GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()