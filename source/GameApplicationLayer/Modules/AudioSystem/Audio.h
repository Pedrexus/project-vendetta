#pragma once

enum class SoundType
{
	FIRST,
	MP3 = FIRST,
	WAVE,
	MIDI,
	OGG,

	// This needs to be the last sound type
	COUNT,
	UNKNOWN,
};