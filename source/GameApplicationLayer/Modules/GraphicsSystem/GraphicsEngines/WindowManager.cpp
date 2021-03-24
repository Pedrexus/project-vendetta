#include "WindowManager.h"
#include <GameViewLayer/WindowProcedure.h>

#include <macros.h>

bool RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION); // TODO: make .ico
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION); // TODO: make .ico
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) GetStockObject(DKGRAY_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = WINDOW_CLASS_NAME;

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

bool WindowManager::InitializeMainWindow()
{
	if(!RegisterWindowClass(m_hAppInst))
		LOG_FATAL("Unable to register window class");

	// Compute window rectangle dimensions based on requested client area dimensions.
	auto rc = CreateWindowRectangle(m_ClientWidth, m_ClientHeight);
	auto width = rc.right - rc.left;
	auto height = rc.bottom - rc.top;

	// fullscreen: CreateWindowExW(WS_EX_TOPMOST, L"Direct3D_11_Win32_GameWindowClass", L"Direct3D 11 Win32 Game", WS_POPUP, ...)
	m_hMainWnd = CreateWindowEx(
		WINDOW_EXTENDED_STYLE,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE_NAME,
		WINDOW_STYLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		m_hAppInst,
		NULL
	);

	if (!m_hMainWnd)
		LOG_FATAL("Unable to create extended window");
	else
	{
		ShowWindow(m_hMainWnd, SW_SHOW);  // nCmdShow
		UpdateWindow(m_hMainWnd);
	}

	return true;
}