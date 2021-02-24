#pragma once

enum GameViewType
{
	Human,
	Remote,
	AI,
	Recorder,
	Other
};

enum BaseGameState
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

struct AppMsg
{
	HWND m_hWnd;
	UINT m_uMsg;
	WPARAM m_wParam;
	LPARAM m_lParam;
};
