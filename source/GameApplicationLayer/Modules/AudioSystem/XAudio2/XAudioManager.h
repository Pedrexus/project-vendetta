#pragma once

#include <XAudio2.h>

#include "../IAudioManager.h"

class XAudioManager : public IAudioManager
{
protected:

	IXAudio2* m_pXAudio2;
	IXAudio2MasteringVoice* m_pMasterVoice;

public:
	XAudioManager() : IAudioManager(), m_pXAudio2(nullptr), m_pMasterVoice(nullptr) {}
	bool IsActive() override { return m_pXAudio2 != nullptr; }

	IAudioBuffer* InitAudioBuffer(std::shared_ptr<ResourceData::Sound> soundData) override;
	void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) override;

	bool Initialize(HWND hWnd) override;
	void Shutdown() override;

};