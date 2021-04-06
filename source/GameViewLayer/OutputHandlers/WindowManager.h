#pragma once

#include <const.h>

class WindowManager
{
	enum WindowState
	{
		COMMON,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN,
		RESIZING,
	};

protected:
	HINSTANCE m_hInstance = nullptr;
	HWND      m_hWnd = nullptr;

	u32		  m_ClientWidth = WINDOW_WIDTH;
	u32		  m_ClientHeight = WINDOW_HEIGHT;

	WindowState m_state = COMMON;

protected:
	void RegisterWindowClass();
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	WindowManager(HINSTANCE hInstance) : m_hInstance(hInstance) {};
	virtual ~WindowManager() { DestroyWindow(m_hWnd); }

	void Initialize();
	inline bool IsReady() { return m_hWnd; }

	HINSTANCE GetAppInst() const { return m_hInstance; }
	HWND GetMainWnd() const { return m_hWnd; }
	std::tuple<u32, u32> GetDimensions() const { return { m_ClientWidth, m_ClientHeight }; }
	f32 GetAspectRatio() const { return static_cast<f32>(m_ClientWidth) / m_ClientHeight; }

	inline void SetMinimize() { m_state = MINIMIZED; }
	inline void SetMaximize() { m_state = MAXIMIZED; }
	inline void SetResizing() { m_state = RESIZING; }
	inline void StopResizing() { m_state = COMMON; }

	inline bool IsMinimized() const { return m_state == MINIMIZED; }
	inline bool IsMaximized() const { return m_state == MAXIMIZED; }
	inline bool IsResizing() const { return m_state == RESIZING; }
};

