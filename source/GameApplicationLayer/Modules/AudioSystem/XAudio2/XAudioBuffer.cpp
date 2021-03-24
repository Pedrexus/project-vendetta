#include "XAudioBuffer.h"

float RescaleVolume(i32 volume)
{
	return (float) volume / (MAXIMUM_VOLUME - MINIMUM_VOLUME);
}

void* XAudioBuffer::Get()
{
	if (!OnRestore())
		return nullptr;
	return m_voice;
}

bool XAudioBuffer::OnRestore()
{
	HRESULT hr;
	if (FAILED(hr = RestoreBuffer()))
		LOG_ERROR("Failed to restore buffer");
	return hr == S_OK;
	// else if (FAILED(hr = FillBufferWithSound())) // The buffer was restored, so we need to fill it with new data
}

bool XAudioBuffer::Play(i32 volume, bool looping)
{
	Stop();

	m_Volume = volume;
	m_isLooping = looping;

	auto voice = (IXAudio2SourceVoice*) Get();
	voice->SetVolume(RescaleVolume(volume));

	auto buffer = GetBuffer();
	if (looping)
		buffer->LoopCount = XAUDIO2_LOOP_INFINITE;

	HRESULT hr;

	if (FAILED(hr = voice->SubmitSourceBuffer(buffer)))
		LOG_ERROR("Failed to add buffer into Source Voice for " + m_soundData->GetName());
	else if (FAILED(hr = voice->Start(0)))
		LOG_ERROR("Failed to start Source Voice for " + m_soundData->GetName());
	
	return hr == S_OK;
}

bool XAudioBuffer::Pause()
{
	auto voice = (IXAudio2SourceVoice*) Get();
	
	HRESULT hr;
	if (FAILED(hr = voice->Stop()))
		LOG_ERROR("Failed to Pause Source Voice for " + m_soundData->GetName());
	else
		m_isPaused = true;
	return hr == S_OK;
}

bool XAudioBuffer::Stop()
{
	auto voice = (IXAudio2SourceVoice*) Get();

	HRESULT hr;
	if (FAILED(hr = voice->Stop()))
		LOG_ERROR("Failed to Stop Source Voice for " + m_soundData->GetName());
	else
		m_isPaused = true;
	return hr == S_OK;
	// TODO: pDSB->SetCurrentPosition(0);	// rewinds buffer to beginning
}

bool XAudioBuffer::Resume()
{
	m_isPaused = false;
	return Play(GetVolume(), IsLooping());
}

bool XAudioBuffer::TogglePause()
{
	return m_isPaused ? Resume() : Pause();
}

bool XAudioBuffer::IsPlaying()
{
	if (m_isPaused)
		return false;

	auto voice = (IXAudio2SourceVoice*) Get();
	
	XAUDIO2_VOICE_STATE state;
	voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);

	return state.BuffersQueued > 0;
}

void XAudioBuffer::SetVolume(i32 volume)
{
	if(volume < MINIMUM_VOLUME || volume > MAXIMUM_VOLUME)
		LOG_ERROR("Volume must be a number between " + std::to_string(MINIMUM_VOLUME) + " and " + std::to_string(MAXIMUM_VOLUME));

	auto voice = (IXAudio2SourceVoice*) Get();
	if (FAILED(voice->SetVolume(RescaleVolume(volume))))
		LOG_ERROR("Failed to set volume for " + m_soundData->GetName());
}

void XAudioBuffer::SetPosition(u32 newPosition) // newPosition is btw 0 and 100
{
	auto voice = (IXAudio2SourceVoice*) Get();
	
	voice->Stop();
	voice->FlushSourceBuffers();

	auto buffer = GetBuffer();
	buffer->LoopCount = IsLooping() ? XAUDIO2_LOOP_INFINITE : 0;
	buffer->PlayBegin = (newPosition * buffer->PlayLength) / 100;
	
	if (FAILED(voice->SubmitSourceBuffer(buffer)))
		LOG_ERROR("Failed to add buffer into Source Voice for " + m_soundData->GetName());
	else if (FAILED(voice->Start(0)))
		LOG_ERROR("Failed to start Source Voice for " + m_soundData->GetName());
}

float XAudioBuffer::GetProgress()
{
	auto voice = (IXAudio2SourceVoice*) Get();

	XAUDIO2_VOICE_STATE state;
	voice->GetState(&state);

	auto context = (XAUDIO2_BUFFER*) state.pCurrentBufferContext;
	return (float) state.SamplesPlayed / context->PlayLength;
}
