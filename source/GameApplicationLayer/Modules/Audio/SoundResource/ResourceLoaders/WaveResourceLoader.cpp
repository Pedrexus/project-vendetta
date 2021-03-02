#include "WaveResourceLoader.h"

ResourceData::Sound* WaveResourceLoader::ParseWave(char* wavStream, size bufferLength, char* writableBuffer)
{
	WAVEFORMATEX waveformatex;

	DWORD file = 0;
	DWORD fileEnd = 0;
	DWORD length = 0;
	DWORD type = 0;
	DWORD pos = 0;

	// mmioFOURCC -- converts four chars into a 4 byte integer code.
	// The first 4 bytes of a valid .wav file is 'R','I','F','F'

	type = *((DWORD*) (wavStream + pos));		pos += sizeof(DWORD);
	if (type != mmioFOURCC('R', 'I', 'F', 'F'))
		return nullptr;

	length = *((DWORD*) (wavStream + pos));	pos += sizeof(DWORD);
	type = *((DWORD*) (wavStream + pos));		pos += sizeof(DWORD);

	// 'W','A','V','E' for a legal .wav file
	if (type != mmioFOURCC('W', 'A', 'V', 'E'))
		return nullptr;		//not a WAV

	// Find the end of the file
	fileEnd = length - 4;

	memset(&waveformatex, 0, sizeof(WAVEFORMATEX));

	bool copiedBuffer = false;

	// Load the .wav format and the .wav data
	// Note that these blocks can be in either order.
	while (file < fileEnd)
	{
		type = *((DWORD*) (wavStream + pos));		pos += sizeof(DWORD);
		file += sizeof(DWORD);

		length = *((DWORD*) (wavStream + pos));	pos += sizeof(DWORD);
		file += sizeof(DWORD);

		switch (type)
		{
			case mmioFOURCC('f', 'a', 'c', 't'):
			{
				LOG_ERROR("This wav file is compressed.  We don't handle compressed wav at this time");
				break;
			}

			case mmioFOURCC('f', 'm', 't', ' '):
			{
				memcpy(&waveformatex, wavStream + pos, length);		pos += length;
				waveformatex.cbSize = (WORD) length;
				break;
			}

			case mmioFOURCC('d', 'a', 't', 'a'):
			{
				copiedBuffer = true;
				if (length != bufferLength)
				{
					LOG_ERROR("Wav resource size does not equal the buffer size");
					return nullptr;
				}
				memcpy(writableBuffer, wavStream + pos, length);
				pos += length;
			}
		}

		file += length;

		// If both blocks have been seen, we can return true.
		if (copiedBuffer)
		{
			milliseconds timeLength = (bufferLength * 1000) / waveformatex.nAvgBytesPerSec;
			auto resourceData = NEW ResourceData::Sound(SoundType::WAVE, waveformatex, timeLength);
			return resourceData;
		}

		// Increment the pointer past the block we just read,
		// and make sure the pointer is word aliged.
		if (length & 1)
		{
			++pos;
			++file;
		}
	}

	// If we get to here, the .wav file didn't contain all the right pieces.
	return nullptr;
}

size WaveResourceLoader::GetLoadedResourceSize(char* rawBuffer, size rawSize)
{
	DWORD file = 0;
	DWORD fileEnd = 0;
	DWORD length = 0;
	DWORD type = 0;
	DWORD pos = 0;

	// mmioFOURCC -- converts four chars into a 4 byte integer code.
	// The first 4 bytes of a valid .wav file is 'R','I','F','F'

	type = *((DWORD*) (rawBuffer + pos));		pos += sizeof(DWORD);
	if (type != mmioFOURCC('R', 'I', 'F', 'F'))
		return 0;

	length = *((DWORD*) (rawBuffer + pos));	pos += sizeof(DWORD);
	type = *((DWORD*) (rawBuffer + pos));		pos += sizeof(DWORD);

	// 'W','A','V','E' for a legal .wav file
	if (type != mmioFOURCC('W', 'A', 'V', 'E'))
		return 0;		//not a WAV

	// Find the end of the file
	fileEnd = length - 4;

	bool copiedBuffer = false;

	// Load the .wav format and the .wav data
	// Note that these blocks can be in either order.
	while (file < fileEnd)
	{
		type = *((DWORD*) (rawBuffer + pos));		pos += sizeof(DWORD);
		file += sizeof(DWORD);

		length = *((DWORD*) (rawBuffer + pos));	pos += sizeof(DWORD);
		file += sizeof(DWORD);

		switch (type)
		{
			case mmioFOURCC('f', 'a', 'c', 't'):
			{
				LOG_ERROR("This wav file is compressed.  We don't handle compressed wav at this time");
				break;
			}

			case mmioFOURCC('f', 'm', 't', ' '):
			{
				pos += length;
				break;
			}

			case mmioFOURCC('d', 'a', 't', 'a'):
			{
				return length;
			}
		}

		file += length;

		// Increment the pointer past the block we just read,
		// and make sure the pointer is word aliged.
		if (length & 1)
		{
			++pos;
			++file;
		}
	}

	// If we get to here, the .wav file didn't contain all the right pieces.
	return 0;
}

ResourceData::Sound* WaveResourceLoader::LoadResource(char* rawBuffer, size rawSize, char* writableBuffer)
{
	if (rawSize <= 0)
		return nullptr;
	
	auto resourceData = ParseWave(rawBuffer, rawSize, writableBuffer);
	if(!resourceData)
		LOG_ERROR("Unable to parse .wav file as WAVE file");

	return resourceData;
}
