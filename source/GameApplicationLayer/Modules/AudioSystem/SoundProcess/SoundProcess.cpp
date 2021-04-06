#include "SoundProcess.h"

#include <GameLogicLayer/Game.h>

void SoundProcess::OnInit()
{
	Process::OnInit();

	// This sound will manage its own handle in the other thread
	IAudioBuffer* buffer = Game::Get()->GetXAudioManager()->InitAudioBuffer(m_SoundData);

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
	m_SoundData = Game::Get()->GetResourceCache()->GetData<ResourceData::Sound>(soundFilename);
	if (!m_SoundData)
		LOG_ERROR("Unable to fetch Resource Data from file " + soundFilename);
};

SoundProcess::~SoundProcess()
{
	if (IsPlaying())
		Stop();

	if (m_AudioBuffer)
		Game::Get()->GetXAudioManager()->ReleaseAudioBuffer(m_AudioBuffer.get());
}