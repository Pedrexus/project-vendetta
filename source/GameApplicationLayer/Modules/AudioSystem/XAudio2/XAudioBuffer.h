#pragma once

#include <const.h>

#include "../IAudioBuffer.h"
#include "../SoundResource/ResourceData/SoundResourceData.h"

class XAudioBuffer : public IAudioBuffer
{
	// HRESULT FillBufferWithSound(); -> Does it have a parallel in XAudio2?
	inline HRESULT RestoreBuffer() { return S_OK; }
	inline XAUDIO2_BUFFER* GetBuffer() { return m_soundData->GetBuffer(); }

protected:
	IXAudio2SourceVoice* m_voice;

public:
	inline XAudioBuffer(IXAudio2SourceVoice* voice, std::shared_ptr<ResourceData::Sound> soundData) :
		IAudioBuffer(soundData),
		m_voice(voice)
	{
		m_voice->SetSourceSampleRate(PRIMARY_SAMPLE_RATE);
	};
	inline ~XAudioBuffer() { m_voice->DestroyVoice(); }

	void* Get() override;
	bool OnRestore() override;
	
	bool Play(i32 volume, bool looping) override;
	bool Pause() override;
	bool Stop() override;
	bool Resume() override;
	
	bool TogglePause() override;
	bool IsPlaying() override;
	void SetVolume(i32 volume) override;
	void SetPosition(u32 newPosition) override;
	
	float GetProgress() override;
};