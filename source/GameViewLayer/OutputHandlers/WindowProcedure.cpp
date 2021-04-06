#include "WindowManager.h"

#include <GameLogicLayer/Game.h>

inline void ResumeOrSuspendOnActivation(WPARAM wParam)
{
	auto game = Game::Get();
	auto isDeactivatingWindow = LOWORD(wParam) == WA_INACTIVE;
	// isDeactivatingWindow ? game->OnSuspending() : game->OnResuming();
}

inline void SuspendOnStartingToResize()
{
	auto game = Game::Get();
	// game->OnSuspending();
	game->GetWindow()->SetResizing();
}

inline void ResumeOnStopResizing()
{
	auto game = Game::Get();
	// game->OnResuming();
	game->GetWindow()->StopResizing();
}

inline void SuspendOnMinimize()
{
	auto game = Game::Get();
	// game->OnSuspending();
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

inline void ResumeAndResize()
{
	auto game = Game::Get();
	// game->OnResuming();
	game->OnResize();
}

inline void RestoreAndResize()
{
	auto game = Game::Get();

	if (game->IsReady())
	{
		if (game->GetWindow()->IsMinimized() || game->GetWindow()->IsMaximized())
			ResumeAndResize();
		else if (game->GetWindow()->IsResizing())
		{
			// If user is dragging the resize bars, we do not resize 
			// the buffers here because as the user continuously 
			// drags the resize bars, a stream of WM_SIZE messages are
			// sent to the window, and it would be pointless (and slow)
			// to resize for each WM_SIZE message received from dragging
			// the resize bars.  So instead, we reset after the user is 
			// done resizing the window and releases the resize bars, which 
			// sends a WM_EXITSIZEMOVE message.
		}
		else
			game->OnResize();
	}
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
			case SIZE_RESTORED: RestoreAndResize();
				break;
			default: 
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
			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
			// Ignore so we don't produce an error beep.
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
		case WM_PAINT:
		case WM_ACTIVATEAPP:
		case WM_POWERBROADCAST:
		case WM_SYSKEYDOWN: // ALT + F4 = system shutdown: WM_SYSCOMMAND message and look for SC_CLOSE in the wParam
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}