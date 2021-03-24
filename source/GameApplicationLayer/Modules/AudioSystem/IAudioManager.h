#pragma once

#include "IAudioBuffer.h"
#include "SoundResource/ResourceData/SoundResourceData.h"

class IAudioManager
{
protected:

	typedef std::list<IAudioBuffer*> AudioBufferList;

	AudioBufferList m_AllSamples;	// List of all currently allocated audio buffers
	bool m_AllPaused;				// Has the sound system been paused?
	bool m_Initialized;				// Has the sound system been initialized?
public:
	IAudioManager() :
		m_Initialized(false),
		m_AllPaused(false)
	{};
	~IAudioManager() { Shutdown(); }

	virtual bool IsActive() = 0;

	virtual IAudioBuffer* InitAudioBuffer(std::shared_ptr<ResourceData::Sound> soundData) = 0;
	virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

	virtual bool Initialize(HWND hWnd) = 0;
	virtual void Shutdown();
	
	virtual void StopAllSounds();
	virtual void PauseAllSounds();
	virtual void ResumeAllSounds();

	bool IsPaused() { return m_AllPaused; }
};