#include "IAudioManager.h"

void IAudioManager::Shutdown()
{
	for (auto audioBuffer : m_AllSamples)
		audioBuffer->Stop();
	m_AllSamples.clear();
}

void IAudioManager::StopAllSounds()
{
	for (auto audioBuffer : m_AllSamples)
		audioBuffer->Stop();
	m_AllPaused = false;
}

void IAudioManager::PauseAllSounds()
{
	for (auto audioBuffer : m_AllSamples)
		audioBuffer->Pause();
	m_AllPaused = true;
}

void IAudioManager::ResumeAllSounds()
{
	for (auto audioBuffer : m_AllSamples)
		audioBuffer->Resume();
	m_AllPaused = false;
}
