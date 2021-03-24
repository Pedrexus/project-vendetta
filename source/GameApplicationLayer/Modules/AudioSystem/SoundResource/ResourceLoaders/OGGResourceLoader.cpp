#include "OGGResourceLoader.h"
#include "OGG/Vorbis.h"

ResourceData::Sound* OGGResourceLoader::ParseOGG(char* oggStream, size bufferSize, std::shared_ptr<ResourceHandle> handle)
{
	OggVorbis_File vf;                     // for the vorbisfile interface

	OggMemoryFile* vorbisMemoryFile = NEW OggMemoryFile;
	vorbisMemoryFile->dataRead = 0;
	vorbisMemoryFile->dataSize = bufferSize;
	vorbisMemoryFile->dataPtr = (unsigned char*) oggStream;

	ov_callbacks oggCallbacks{};
	oggCallbacks.read_func = Vorbis::fread;
	oggCallbacks.seek_func = Vorbis::fseek;
	oggCallbacks.tell_func = Vorbis::ftell;
	oggCallbacks.close_func = Vorbis::fclose;

	auto ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, nullptr, 0, oggCallbacks);
	ASSERT(ov_ret >= 0);

	// ok now the tricky part
	// the vorbis_info struct keeps the most of the interesting format info
	vorbis_info* vi = ov_info(&vf, -1);

	WAVEFORMATEX waveformatex{};
	memset(&waveformatex, 0, sizeof(waveformatex));

	waveformatex.cbSize = sizeof(waveformatex);
	waveformatex.nChannels = vi->channels;
	waveformatex.wBitsPerSample = 16;                    // ogg vorbis is always 16 bit
	waveformatex.nSamplesPerSec = vi->rate;
	waveformatex.nAvgBytesPerSec = waveformatex.nSamplesPerSec * waveformatex.nChannels * 2;
	waveformatex.nBlockAlign = 2 * waveformatex.nChannels;
	waveformatex.wFormatTag = 1;

	DWORD   size = 4096 * 16;
	DWORD   pos = 0;
	int     sec = 0;
	int     ret = 1;

	DWORD bytes = (DWORD) ov_pcm_total(&vf, -1);
	bytes *= 2 * vi->channels;

	if (bufferSize != bytes)
	{
		LOG_ERROR("The Ogg size does not match the memory buffer size!");

		ov_clear(&vf);
		SAFE_DELETE(vorbisMemoryFile);
		return nullptr;
	}

	// now read in the bits
	// Any time you have a while loop that might take some time, such as
	// decompressing a large OGG file, it’s a good idea to create a callback function
	while (ret && pos < bytes)
	{
		// If you feel sufficiently energetic one weekend, this is where you’ll want to play
		// around if you’d like to implement decompression of the OGG stream in real time.  
		// pg. 408
		ret = ov_read(&vf, handle->WritableBuffer() + pos, size, 0, 2, 1, &sec);
		pos += ret;
		if (bytes - pos < size)
			size = bytes - pos;
	}

	XAUDIO2_BUFFER xaudio2buffer = { 0 };
	xaudio2buffer.AudioBytes = size;  //size of the audio buffer in bytes
	xaudio2buffer.pAudioData = (BYTE*) handle->Buffer();  //buffer containing audio data
	xaudio2buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	auto timeLength = (milliseconds)(1000.f * ov_time_total(&vf, -1));

	ov_clear(&vf);

	SAFE_DELETE(vorbisMemoryFile);

	auto resourceData = NEW ResourceData::Sound(SoundType::OGG, waveformatex, xaudio2buffer, timeLength);
	return resourceData;
}

size OGGResourceLoader::GetLoadedResourceSize(char* rawBuffer, size rawSize)
{
	OggVorbis_File vf;                     // for the vorbisfile interface

	OggMemoryFile* vorbisMemoryFile = NEW OggMemoryFile;
	vorbisMemoryFile->dataRead = 0;
	vorbisMemoryFile->dataSize = rawSize;
	vorbisMemoryFile->dataPtr = (unsigned char*) rawBuffer;

	ov_callbacks oggCallbacks{};
	oggCallbacks.read_func = Vorbis::fread;
	oggCallbacks.seek_func = Vorbis::fseek;
	oggCallbacks.tell_func = Vorbis::ftell;
	oggCallbacks.close_func = Vorbis::fclose;

	auto ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, nullptr, 0, oggCallbacks);
	ASSERT(ov_ret >= 0);

	// ok now the tricky part
	// the vorbis_info struct keeps the most of the interesting format info
	vorbis_info* vi = ov_info(&vf, -1);

	DWORD   size = 4096 * 16;
	DWORD   pos = 0;
	int     sec = 0;
	int     ret = 1;

	DWORD bytes = (DWORD) ov_pcm_total(&vf, -1);
	bytes *= 2 * vi->channels;

	ov_clear(&vf);

	SAFE_DELETE(vorbisMemoryFile);

	return bytes;
}

ResourceData::Sound* OGGResourceLoader::LoadResource(char* rawBuffer, size rawSize, std::shared_ptr<ResourceHandle> handle)
{
	if (rawSize <= 0)
		return nullptr;

	auto resourceData = ParseOGG(rawBuffer, rawSize, handle);
	if (!resourceData)
		LOG_ERROR("Unable to parse .ogg file as Vorbis/WAVE file");

	return resourceData;
}
