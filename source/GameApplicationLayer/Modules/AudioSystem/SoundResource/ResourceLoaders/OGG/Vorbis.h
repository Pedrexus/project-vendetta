#pragma once

#include <pch.h>
#include <types.h>
#include <macros.h>

// a structure that will keep track of the memory buffer
struct OggMemoryFile
{
    unsigned char* dataPtr; // Pointer to the data in memory
    size_t    dataSize;     // Size of the data
    size_t    dataRead;     // Bytes read so far

    OggMemoryFile() : dataPtr(nullptr), dataSize(0), dataRead(0) {};
};

namespace Vorbis
{
    size_t fread(void* data_ptr, size_t byteSize, size_t sizeToRead, void* data_src)
    {
        OggMemoryFile* pVorbisData = static_cast<OggMemoryFile*>(data_src);
        if (!pVorbisData)
            return -1;

        auto actualSizeToRead = sizeToRead * byteSize;
        auto spaceToEOF = pVorbisData->dataSize - pVorbisData->dataRead;

        if (actualSizeToRead > spaceToEOF)
            actualSizeToRead = spaceToEOF;

        memcpy(data_ptr, (char*) pVorbisData->dataPtr + pVorbisData->dataRead, actualSizeToRead);
        pVorbisData->dataRead += actualSizeToRead;

        return actualSizeToRead;
    }

	i32 fseek(void* data_src, ogg_int64_t offset, i32 origin)
	{
		OggMemoryFile* pVorbisData = static_cast<OggMemoryFile*>(data_src);
		if (!pVorbisData)
			return -1;

		switch (origin)
		{
			case SEEK_SET:
			{
				auto actualOffset = (ogg_int64_t)pVorbisData->dataSize >= offset ? offset : pVorbisData->dataSize;
				pVorbisData->dataRead = static_cast<size>(actualOffset);
				break;
			}

			case SEEK_CUR:
			{
				auto spaceToEOF = pVorbisData->dataSize - pVorbisData->dataRead;
				auto actualOffset = offset < (ogg_int64_t)spaceToEOF ? offset : spaceToEOF;
				pVorbisData->dataRead += static_cast<size>(actualOffset);
				break;
			}

			case SEEK_END:
			{
				pVorbisData->dataRead = pVorbisData->dataSize + 1;
				break;
			}

			default:
				LOG_ERROR("Bad parameter for 'origin', requires same as fseek.");
				break;
		};

		return 0;
	}

	i32 fclose(void* data_src)
	{
		// Do nothing - we assume someone else is managing the raw buffer (Resource Cache)
		return 0;
	}

	long ftell(void* data_src)
	{
		OggMemoryFile* pVorbisData = static_cast<OggMemoryFile*>(data_src);
		if (!pVorbisData)
			return -1L;
		return static_cast<long>(pVorbisData->dataRead);
	}

}