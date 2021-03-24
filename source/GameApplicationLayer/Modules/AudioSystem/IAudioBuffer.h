#pragma once

#include <pch.h>
#include <types.h>

#include "../ResourceCache.h"
#include "SoundResource/ResourceData/SoundResourceData.h"

class IAudioBuffer
{
protected:
	std::shared_ptr<ResourceData::Sound> m_soundData;
	bool m_isPaused; 
	bool m_isLooping;
	i32 m_Volume;

	IAudioBuffer(std::shared_ptr<ResourceData::Sound> soundData) :
		m_soundData(soundData),
		m_isPaused(false),
		m_isLooping(false),
		m_Volume(0)
	{}

public:
	virtual ~IAudioBuffer() = default;

	virtual void* Get() = 0;
	virtual std::shared_ptr<ResourceData::Sound> GetResourceData() { return m_soundData; };
	virtual bool OnRestore() = 0;

	virtual bool Play(i32 volume, bool looping) = 0;
	virtual bool Pause() = 0;
	virtual bool Stop() = 0;
	virtual bool Resume() = 0;

	virtual bool TogglePause() = 0;
	virtual bool IsPlaying() = 0;
	virtual bool IsLooping() const { return m_isLooping; };
	virtual void SetVolume(i32 volume) = 0;
	virtual void SetPosition(u32 newPosition) = 0;
	virtual i32 GetVolume() const { return m_Volume; };
	virtual f32 GetProgress() = 0;
};