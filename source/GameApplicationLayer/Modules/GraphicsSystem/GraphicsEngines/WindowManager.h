#pragma once

#include <const.h>

class WindowManager
{
protected:
	HINSTANCE m_hAppInst;	// application instance handle
	HWND      m_hMainWnd;	// main window handle
	bool      m_AppPaused;	// is the application paused?
	bool      m_Minimized;	// is the application minimized?
	bool      m_Maximized;	// is the application maximized?
	bool      m_Resizing;	// are the resize bars being dragged?
	bool      m_FullscreenState;	// fullscreen enabled
	u32		  m_ClientWidth;
	u32		  m_ClientHeight;

	bool InitializeMainWindow();

public:
	inline WindowManager(HINSTANCE hInstance) :
		m_hAppInst(hInstance),
		m_hMainWnd(nullptr),
		m_AppPaused(false),
		m_Minimized(false),
		m_Maximized(false),
		m_Resizing(false),
		m_FullscreenState(false),
		m_ClientWidth(WINDOW_WIDTH),
		m_ClientHeight(WINDOW_HEIGHT)
	{};
	virtual inline ~WindowManager() { DestroyWindow(m_hMainWnd); }

	HINSTANCE GetAppInst() const { return m_hAppInst; }
	HWND GetMainWnd() const { return m_hMainWnd; }
	f32 AspectRatio() const { return static_cast<f32>(m_ClientWidth) / m_ClientHeight; }

	inline void SetMinimize() { m_Minimized = true; m_Maximized = false; }
	inline void SetMaximize() { m_Maximized = true; m_Minimized = false; }
	inline void SetResizing(bool resizing) { m_Resizing = resizing; }

	inline bool IsMinimized() const { return m_Minimized; }
	inline bool IsMaximized() const { return m_Maximized; }
	inline bool IsResizing() const { return m_Resizing; }
};

