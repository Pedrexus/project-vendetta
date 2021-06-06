#include "WindowManager.h"

#include <GameLogicLayer/Game.h>

#include <DirectXTK12/Keyboard.h>
#include <DirectXTK12/Mouse.h>

inline void GetMinMaxWindowSizeInfo(LPARAM lParam)
{
	auto minmaxinfo = (MINMAXINFO*) lParam;
	minmaxinfo->ptMinTrackSize.x = 200;
	minmaxinfo->ptMinTrackSize.y = 200;
}

inline void PaintWindow(HWND& hWnd)
{
	static PAINTSTRUCT ps;
	static HDC hdc;

	hdc = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

inline LRESULT AvoidErrorBeep()
{
	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	// Ignore so we don't produce an error beep.
	static auto lresult = MAKELRESULT(0, MNC_CLOSE);
	return lresult;
}

inline void ToggleFullscreen(HWND& hWnd, WPARAM& wParam, LPARAM& lParam)
{
	static bool s_fullscreen = false;
	// TODO: Game::SetFullscreen(), Game::SetWindowed()

	static u16 width = 800;
	static u16 height = 600;

	if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
	{
		// Implements the classic ALT+ENTER fullscreen toggle
		if (s_fullscreen)
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

			/*if (sample)
				sample->GetDefaultSize(width, height);*/

			ShowWindow(hWnd, SW_SHOWNORMAL);

			SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		else
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, 0);
			SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

			SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

			ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		}

		s_fullscreen = !s_fullscreen;
	}
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
		case WM_MENUCHAR: return AvoidErrorBeep();
		case WM_GETMINMAXINFO: GetMinMaxWindowSizeInfo(lParam);
			return 0;
		case WM_PAINT: PaintWindow(hWnd);
			break;
		case WM_ACTIVATEAPP:
			DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			DirectX::Mouse::ProcessMessage(message, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
			break;
		case WM_POWERBROADCAST: // TODO: power changes
		case WM_SYSKEYDOWN: 
			// ALT + F4 = system shutdown: WM_SYSCOMMAND message and look for SC_CLOSE in the wParam
			ToggleFullscreen(hWnd, wParam, lParam);
			DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
			break;
		case WM_MOVE: // TODO: OnWindowMoved
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}