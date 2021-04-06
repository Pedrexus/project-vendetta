#include "WindowManager.h"

#include <macros.h>
#include <Helpers/Functions.h>

void WindowManager::RegisterWindowClass()
{
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION); // TODO: make .ico
	wc.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&wc))
		LOG_FATAL("Failed to register extended window class");
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

std::tuple<INT, INT> GetWindowDimensions(INT screenWidth, INT screenHeight)
{
	auto rc = CreateWindowRectangle(screenWidth, screenHeight);
	auto width = rc.right - rc.left;
	auto height = rc.bottom - rc.top;

	return { width, height };
}

HWND CreateOverlappedWindow(HINSTANCE hInstance, INT screenWidth, INT screenHeight)
{
	return CreateWindow(
		WINDOW_CLASS_NAME, WINDOW_TITLE_NAME, WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight, NULL, NULL, hInstance, NULL
	);
}

void WindowManager::Initialize()
{
	RegisterWindowClass();

	u32 width, height;
	std::tie(width, height) = GetWindowDimensions(m_ClientWidth, m_ClientHeight);
	m_hWnd = CreateOverlappedWindow(m_hInstance, width, height);

	if (!m_hWnd)
		LOG_FATAL("Unable to create extended window");
	else
	{
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
	}
}
