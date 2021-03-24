#include <pch.h>
#include <macros.h>

#include "XAudioManager.h"
#include "XAudioBuffer.h"

#include "../SoundResource/ResourceData/SoundResourceData.h"

IAudioBuffer* XAudioManager::InitAudioBuffer(std::shared_ptr<ResourceData::Sound> soundData)
{
	if (!m_Initialized)
		return nullptr;

	switch (soundData->GetSoundType())
	{
		case SoundType::OGG:
		case SoundType::WAVE:
			// We support WAVs and OGGs
			break;

		case SoundType::MP3:
		case SoundType::MIDI:	//If it's a midi file, then do nothin at this time... maybe we will support this in the future
			LOG_ERROR("MP3s and MIDI are not supported");
			return nullptr;
			break;

		default:
			LOG_ERROR("Unknown sound type");
			return nullptr;
	}

	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(m_pXAudio2->CreateSourceVoice(&pSourceVoice, soundData->GetFormat())))
		LOG_ERROR("Failed to create Source Voice for " + soundData->GetName());

	// Add handle to the list
	IAudioBuffer* audioBuffer = NEW XAudioBuffer(pSourceVoice, soundData);
	m_AllSamples.push_front(audioBuffer);

	return audioBuffer;
}

void XAudioManager::ReleaseAudioBuffer(IAudioBuffer* audioBuffer)
{
	audioBuffer->Stop();
	m_AllSamples.remove(audioBuffer);
}

bool XAudioManager::Initialize(HWND hWnd)
{
	if (m_Initialized)
		return true;

	m_Initialized = false;
	m_AllSamples.clear();

	SAFE_RELEASE(m_pXAudio2);

	HRESULT hr;

	if (FAILED(hr = XAudio2Create(&m_pXAudio2, NULL, XAUDIO2_DEFAULT_PROCESSOR)))
		LOG_FATAL("Failed to create instance of the XAudio2 engine");
	else if (FAILED(hr = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
		LOG_FATAL("Failed to create the Mastering Voice around the Audio Device");
	else
		m_Initialized = true;
	return hr == S_OK;
}

void XAudioManager::Shutdown()
{
	IAudioManager::Shutdown();

	SAFE_RELEASE(m_pXAudio2);
	SAFE_DELETE(m_pMasterVoice);

	m_Initialized = false;
}
