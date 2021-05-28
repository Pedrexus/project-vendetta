#pragma once

#include <XAudio2.h>

#include "../../Audio.h"
#include "../../../ResourceCache.h"

/*
	A commercial grade sound system would only load the
	compressed sound into memory and use a thread to decompress bits of it as it is played,
	saving a ton of memory.
*/
namespace ResourceData
{
    class Sound : public IResourceData
    {
		friend class WaveResourceLoader;
		friend class OggResourceLoader;

	protected:
		SoundType m_SoundType;				// is this an Ogg, WAV, etc.?
		bool m_Initialized;					// has the sound been initialized - means it's ready to play
		WAVEFORMATEX m_WavFormatEx;			// description of the PCM format
		XAUDIO2_BUFFER m_xaudio2buffer;
		milliseconds m_LengthMilliseconds;			// how long the sound is in milliseconds
		std::string m_name;

	public:
		inline Sound(SoundType soundType, WAVEFORMATEX WavFormatEx, XAUDIO2_BUFFER XAudio2Buffer, milliseconds timeLength) :
			m_SoundType(soundType),
			m_Initialized(false),
			m_WavFormatEx(WavFormatEx),
			m_xaudio2buffer(XAudio2Buffer),
			m_LengthMilliseconds(timeLength)
		{}; // don't do anything yet - timing sound Initialization is important!
		inline ~Sound() = default;
		inline SoundType GetSoundType() { return m_SoundType; }
		inline WAVEFORMATEX const* GetFormat() { return &m_WavFormatEx; }
		inline XAUDIO2_BUFFER* GetBuffer() { return &m_xaudio2buffer; }
		inline milliseconds GetLengthMilliseconds() const { return m_LengthMilliseconds; }
    };
}