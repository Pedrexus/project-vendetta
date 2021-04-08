#include "WindowManager.h"

#include <GameLogicLayer/Game.h>

inline void GetMinMaxWindowSizeInfo(LPARAM lParam)
{
	auto minmaxinfo = (MINMAXINFO*) lParam;
	minmaxinfo->ptMinTrackSize.x = 200;
	minmaxinfo->ptMinTrackSize.y = 200;
}

inline LRESULT AvoidErrorBeep()
{
	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	// Ignore so we don't produce an error beep.
	static auto lresult = MAKELRESULT(0, MNC_CLOSE);
	return lresult;
}

inline void ResumeOrSuspendOnActivation(WPARAM wParam)
{
	auto game = Game::Get();
	auto isDeactivatingWindow = LOWORD(wParam) == WA_INACTIVE;
	isDeactivatingWindow ? game->OnSuspending() : game->OnResuming();
}

inline void SuspendOnStartingToResize()
{
	auto game = Game::Get();
	game->OnSuspending();
	game->GetWindow()->SetResizing();
}

inline void ResumeOnStopResizing()
{
	auto game = Game::Get();
	game->OnResuming();
	game->GetWindow()->StopResizing();
	game->OnResize();
}

inline void SuspendOnMinimize()
{
	auto game = Game::Get();
	game->OnSuspending();
	game->GetWindow()->SetMinimize();
}

inline void ResizeOnMaximize(LPARAM lParam)
{
	auto game = Game::Get();
	game->GetWindow()->SetMaximize();

	auto width = LOWORD(lParam);
	auto height = HIWORD(lParam);
	game->OnResize(width, height);
}

inline void ResumeAndResize(LPARAM lParam)
{
	auto game = Game::Get();
	game->OnResuming();
	game->GetWindow()->StopResizing();

	auto width = LOWORD(lParam);
	auto height = HIWORD(lParam);
	game->OnResize(width, height);
}

void ResizeWindow(WPARAM wParam, LPARAM lParam)
{
	auto game = Game::Get();
	if (game->IsReady())
	{
		switch (wParam)
		{
			case SIZE_MINIMIZED: SuspendOnMinimize();
				break;
			case SIZE_MAXIMIZED: ResizeOnMaximize(lParam);
				break;
			case SIZE_RESTORED: ResumeAndResize(lParam);
				break;
		}

	}
}

LRESULT CALLBACK WindowManager::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_ACTIVATE: ResumeOrSuspendOnActivation(wParam);
			return 0;
		case WM_ENTERSIZEMOVE: SuspendOnStartingToResize();
			return 0;
		case WM_EXITSIZEMOVE: ResumeOnStopResizing();
			return 0;
		case WM_SIZE: ResizeWindow(wParam, lParam);
			return 0;
		case WM_DESTROY: PostQuitMessage(0);
			break;
		case WM_MENUCHAR: 
			return AvoidErrorBeep();
		case WM_GETMINMAXINFO: GetMinMaxWindowSizeInfo(lParam);
			return 0;
		case WM_PAINT:
		case WM_ACTIVATEAPP:
		case WM_POWERBROADCAST:
		case WM_SYSKEYDOWN: // ALT + F4 = system shutdown: WM_SYSCOMMAND message and look for SC_CLOSE in the wParam
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}