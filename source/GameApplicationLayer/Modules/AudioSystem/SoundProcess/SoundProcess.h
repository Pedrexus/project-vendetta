#pragma once

#include <pch.h>
#include <types.h>

#include "../../ProcessSystem.h"
#include "../../AudioSystem.h"

/////////////////////////////////////////////////////////////////////////////
// class SoundProcess
//
//    A Sound Process, not to be confused with a Sound Resource (SoundResource)
//	  manages a sound as it is being played. You can use this class to manage
//    timing between sounds & animations.
//
/////////////////////////////////////////////////////////////////////////////

class SoundProcess : public Process
{
protected:
	std::shared_ptr<ResourceData::Sound> m_SoundData;
	std::shared_ptr<IAudioBuffer> m_AudioBuffer;		// handle to the implementation dependent audio buffer (DSound, Miles) 

	// these hold the initial setting until the sound is actually launched.
	i32  m_InitialVolume;
	bool m_InitialIsLooping;

	void OnInit() override;
	void OnUpdate(milliseconds dt) override;

	// TODO: void InitializeVolume();
	SoundProcess() = default;	 // Disable Default Construction

public:
	SoundProcess(std::string soundFilename, i32 volume = 100, bool looping = false);
	virtual ~SoundProcess();

	// XAudioBuffer composition
	inline void Play(const i32 volume = 100, const bool looping = false) { m_AudioBuffer->Play(volume, looping); }
	inline void Stop() { m_AudioBuffer->Stop(); }
	inline void SetVolume(i32 volume) { m_AudioBuffer->SetVolume(volume); }
	inline i32 GetVolume() { return m_AudioBuffer->GetVolume(); }
	inline bool IsPlaying() { return m_AudioBuffer->IsPlaying(); }
	inline bool IsLooping() { return m_AudioBuffer && m_AudioBuffer->IsLooping(); }
	inline f32 GetProgress() { return m_AudioBuffer->GetProgress(); };
	inline void TogglePause() { m_AudioBuffer->TogglePause(); }

	// ResourceData::Sound composition
	inline milliseconds GetTimeLength() { m_SoundData->GetLengthMilliseconds(); };
};