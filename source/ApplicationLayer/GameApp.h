#pragma once

#include <pch.h>

// define class in header, implement in cpp

/*
	Game Application Layer

	Responsibilities:
	- operating system–specific tasks
	- interface with the hardware and operating system
	- handling the application life cycle
	- access to localized strings
	- resource initialization
	- game initialization

	Subsystems:
	- user-presented strings (i18n)
	- game logic
	- game options
	- resource cache (loads textures, meshes, sounds, etc.)
	- main event manager (subsystem communication)
	- network manager
*/
class GameApp
{
	// handle to a window
	HWND m_window;

	// set of features targeted by a Direct3D device
	D3D_FEATURE_LEVEL m_featureLevel;

	// represents a virtual adapter; it is used to create resources.
	Microsoft::WRL::ComPtr<ID3D11Device5> m_d3dDevice;

	// represents a device context which generates rendering commands
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_d3dContext;
	
	// implements one or more surfaces (IDXGISurface) for storing rendered data before presenting it to an output.
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

	// identifies the render-target subresources that can be accessed during rendering
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	// accesses a texture resource during depth-stencil testing
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	TCHAR m_saveGameDirectory[MAX_PATH];

public:
	GameApp();
	~GameApp() = default;

	/*
		App::Initialize

		Responsibilities:
		- Detects multiple instances of the application.
		- Checks secondary storage space and memory.
		- Calculates the CPU speed.
		- Loads the game’s resource cache.
		- Loads strings that will be presented to the player.
		- Creates the LUA script manager.
		- Creates the game’s Event Manager.
		- Uses the script manager to load initial game options.
		- Initializes DirectX, the application’s window, and the D3D device.
		- Creates the game logic and game views.
		- Sets the directory for save games and other temporary files.
		- Preloads selected resources from the resource cache.
	*/
	virtual bool Initialize(
		HINSTANCE hInstance,
		LPWSTR lpCmdLine,
		HWND hWnd,
		INT nCmdShow
	);

	// Shutdown sequence occurs in reverse order from initialization
	virtual LRESULT Shutdown();

	// create the resources that depend on the device.
	virtual void CreateDevice();

	virtual HWND InitializeWindow(HINSTANCE hInstance, INT nCmdShow, INT screenWidth, INT screenHeight);
};

extern GameApp* g_GameApp;