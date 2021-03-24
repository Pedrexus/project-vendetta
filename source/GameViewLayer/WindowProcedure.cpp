#include "WindowProcedure.h"

void ResizeWindow(WPARAM wParam, LPARAM lParam)
{
	auto game = GameApp::Get();

	// Save the new client area dimensions.
	auto width = LOWORD(lParam);
	auto height = HIWORD(lParam);

	if (game->IsReady())
	{
		if (wParam == SIZE_MINIMIZED)
		{
			game->OnSuspending();
			game->GetWindow()->SetMinimize(); // game->OnResize(minimized = true);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			game->GetWindow()->SetMaximize();
			game->GetWindow()->OnResize(width, height);
		}
		else if (wParam == SIZE_RESTORED)
		{
			// Restoring from minimized state?
			if (game->GetWindow()->IsMinimized())
			{
				game->OnResuming();
				game->GetWindow()->OnResize();
			}

			// Restoring from maximized state?
			else if (game->GetWindow()->IsMaximized())
			{
				game->OnResuming();
				game->GetWindow()->OnResize();
			}
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
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				game->GetWindow()->OnResize();
			}
		}
	}
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto game = GameApp::Get();

	// ALT + F4 = system shutdown: WM_SYSCOMMAND message and look for SC_CLOSE in the wParam
	switch (message)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				game->OnSuspending();
			else
				game->OnResuming();
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			ResizeWindow(wParam, lParam);
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			game->OnSuspending();
			game->GetWindow()->SetResizing(true);
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			game->OnResuming();
			game->GetWindow()->SetResizing(false);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Ignore so we don't produce an error beep.
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			if (lParam)
			{
				auto info = (MINMAXINFO*) lParam;
				info->ptMinTrackSize.x = 320;
				info->ptMinTrackSize.y = 200;
			}
			break;

			// TODO: below cases still have to be worked on
		case WM_PAINT:
			PAINTSTRUCT ps;
			(void) BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_ACTIVATEAPP:
			if (game)
			{
				if (wParam)
				{
					noop; // game->OnActivated();
				}
				else
				{
					noop; // game->OnDeactivated();
				}
			}
			break;

		case WM_POWERBROADCAST:
			switch (wParam)
			{
				case PBT_APMQUERYSUSPEND:
					return TRUE;

				case PBT_APMRESUMESUSPEND:
					return TRUE;
			}
			break;

		case WM_SYSKEYDOWN:
			if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
			{
				// Implements the classic ALT+ENTER fullscreen toggle
				if (false)
				{
					SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
					SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

					int width = 800;
					int height = 600;
					if (game)
						noop; // game->GetDefaultSize(width, height);

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
			}
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}