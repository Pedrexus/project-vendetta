#pragma once

enum GameViewType
{
	Human,
	Remote,
	AI,
	Recorder,
	Other
};

enum GameState
{
	Invalid,
	Initializing,
	MainMenu,
	WaitingForPlayers,
	LoadingGameEnvironment,
	WaitingForPlayersToLoadEnvironment,
	SpawningPlayersActors,
	Running
};