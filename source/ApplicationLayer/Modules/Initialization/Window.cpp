#include "../Initialization.h"

#include <const.h>
#include <macros.h>

// How to move this to GameApp?
LRESULT CALLBACK WindowProcedure(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
    // TODO: Set s_fullscreen to true if defaulting to fullscreen.

    // auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    int game = NULL;

    // ALT + F4 = system shutdown: WM_SYSCOMMAND message and look for SC_CLOSE in the wParam
    switch (message)
    {
    case WM_PAINT:
        if (s_in_sizemove)
        {
            noop; // game->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend)
                    // TODO: take a look Chapter 5 p. 149
                    noop; // game->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && game)
                noop; // game->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && game)
        {
            noop; // game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (game)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            noop; // game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        if (lParam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
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
            if (!s_in_suspend && game)
                noop; // game->OnSuspending();
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && game)
                    noop; // game->OnResuming();
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
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

            s_fullscreen = !s_fullscreen;
        }
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool RegisterWindowClass(HINSTANCE hInstance)
{
    // Register class
    WNDCLASSEX wcex = {};

    // byte size of the struct
    wcex.cbSize = sizeof(WNDCLASSEX);

    // class style
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    // a callback function to the window procedure
    wcex.lpfnWndProc = WindowProcedure;

    // a handle to the instance that contains the window procedure for the class.
    wcex.hInstance = hInstance;

    // window icon
    // TODO: make .ico
    wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");

    // window arrow cursor
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    // window background color brush
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND + 1);

    // window class name
    wcex.lpszClassName = WINDOW_CLASS_NAME;

    // window small icon
    wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
    
    // registers a window class for subsequent use in calls to the CreateWindow function
    return RegisterClassEx(&wcex);
}

RECT CreateWindowRectangle(INT screenWidth, INT screenHeight)
{   
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-features#overlapped-windows
    //
    // An overlapped window is a top-level window (non-child window) that has a title bar,
    // border, and client area; it is meant to serve as an application's main window. 
    // It can also have a window menu, minimize and maximize buttons, and scroll bars. 
    // An overlapped window used as a main window typically includes all of these components.

    RECT rc = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };

    // Calculates the required size of the window rectangle
    AdjustWindowRect(&rc, WINDOW_STYLE, WINDOW_HAS_MENU);

    return rc;
}