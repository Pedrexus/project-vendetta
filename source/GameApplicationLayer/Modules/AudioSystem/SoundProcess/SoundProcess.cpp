#include "SoundProcess.h"

#include "../../../GameApp.h"

void SoundProcess::OnInit()
{
	Process::OnInit();

	// This sound will manage its own handle in the other thread
	IAudioBuffer* buffer = GameApp::GetXAudioManager()->InitAudioBuffer(m_SoundData);

	if (!buffer)
	{
		Fail();
		return;
	}

	m_AudioBuffer.reset(buffer);

	Play(m_InitialVolume, m_InitialIsLooping);
}

void SoundProcess::OnUpdate(milliseconds dt)
{
	if (!IsPlaying())
		Succeed();
}

SoundProcess::SoundProcess(std::string soundFilename, i32 volume, bool looping) :
	m_InitialVolume(volume),
	m_InitialIsLooping(looping)
{
	m_SoundData = GameApp::GetResourceCache()->GetData<ResourceData::Sound>(soundFilename);
	if (!m_SoundData)
		LOG_ERROR("Unable to fetch Resource Data from file " + soundFilename);
};

SoundProcess::~SoundProcess()
{
	if (IsPlaying())
		Stop();

	if (m_AudioBuffer)
		GameApp::GetXAudioManager()->ReleaseAudioBuffer(m_AudioBuffer.get());
}