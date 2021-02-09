// edit Properties -> VC++ Directories -> Include Directories
#include "GameApp.h"
#include "Initialization.h"

#include <const.h>

// smart pointer type
// maintains a reference count 
// releases the interface when count goes to zero.
using Microsoft::WRL::ComPtr;

// global GameApp pointer
GameApp* g_GameApp = nullptr;

GameApp::GameApp() :
    m_window(nullptr),
    m_d3dDevice(nullptr),
    m_d3dContext(nullptr),
    m_swapChain(nullptr),
    m_renderTargetView(nullptr),
    m_depthStencilView(nullptr),
    m_featureLevel(D3D_FEATURE_LEVEL_11_1),
    m_saveGameDirectory(L""),
    m_ResCache(nullptr)
{
	// only the game (GameApp subclass) calls the constructor

	// sets the global pointer
	g_GameApp = this;
};

bool GameApp::Initialize(
	HINSTANCE hInstance,
	LPWSTR lpCmdLine,
	HWND hWnd,
	INT nCmdShow
)
{
	// Check for existing instance of the same window
	// TODO: create a splash screen to help minimize this problem
	if (!IsOnlyInstance(GAME_TITLE))
		return false;

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor(NULL);

	if (!CheckStorage(DISK_SPACE))
		return false;

	if(!CheckMemory(PHYSICAL_RAM, VIRTUAL_RAM))
		return false;

	if (!CheckCPUSpeed(CPU_SPEED))
		return false;

	// TODO: development editor resource cache initialization - chapter 22
    IResourceFile* zipFile = new ResourceZipFile(RESOURCES_ZIPFILE);
    m_ResCache = new ResourceCache(RESOURCES_SIZE, zipFile);

    if (!m_ResCache->Init())
        return false;

	// TODO: load strings - chapter 8

	// TODO: event manager and event registering - Chapter 11
	
	// TODO: Lua script manager initialization - Chapter 12

	// TODO: window must be a method
	// window creation and initialization
    m_window = InitializeWindow(hInstance, nCmdShow, WINDOW_WIDTH, WINDOW_HEIGHT);

    CreateDevice();

    // TODO: create game logic & view - Chapter 21

    // copy the string ptr
    _tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(SAVE_GAME_DIR));

    // now that all the major systems are initialized, preload resources - Chapter 8

	return true;
};

LRESULT GameApp::Shutdown()
{
    /*
        The creation order was:
            1. resource cache
            2. game window second
            3. game logic object third
        
        Release happens in reverse order
    */

    // SAFE_DELETE(m_pGame);
    DestroyWindow(m_window);
    // VDestroyNetworkEventForwarder();
    // SAFE_DELETE(m_pBaseSocketManager);
    // SAFE_DELETE(m_pEventManager);
    // BaseScriptComponent::UnregisterScriptFunctions();
    // ScriptExports::Unregister();
    // LuaStateManager::Destroy();
    // SAFE_DELETE(m_ResCache);

    return 0;
}


void GameApp::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // defines which graphics cards the game will support
    // here, only graphics that support DX11 and DX11.1
    // would be able to run the game.
    static const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    // Creates a device that represents the display adapter.
    if (FAILED(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
    )))
        throw std::exception("Unable to create Direct 3D 11 device");

#ifdef _DEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // ComPtr .As: attaches a new ComPtr variable pointing to the value (doesn't need to be the same type)
    if (FAILED(device.As(&m_d3dDevice)))
        throw std::exception("Unable to return Direct 3D 11 device");
    if (FAILED(context.As(&m_d3dContext)))
        throw std::exception("Unable to return Direct 3D 11 context");

    // TODO: Initialize device dependent objects here (independent of window size).
}

HWND GameApp::InitializeWindow(HINSTANCE hInstance, INT nCmdShow, INT screenWidth, INT screenHeight)
{
    // Register class
    if (!RegisterWindowClass(hInstance))
        throw std::exception("Unable to register window class");

    // Create window rectangle
    auto rc = CreateWindowRectangle(screenWidth, screenHeight);

    // Create window
    // fullscreen: CreateWindowExW(WS_EX_TOPMOST, L"Direct3D_11_Win32_GameWindowClass", L"Direct3D 11 Win32 Game", WS_POPUP, ...)
    HWND hwnd = CreateWindowExW(
        WINDOW_EXTENDED_STYLE,
        WINDOW_CLASS_NAME,
        WINDOW_TITLE_NAME,
        WINDOW_STYLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left, // width
        rc.bottom - rc.top, // height
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        throw std::exception("Unable to create extended window");

    // Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.
    ShowWindow(hwnd, nCmdShow);

    // TODO: understand this
    // SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

    GetClientRect(hwnd, &rc);

    // g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            ; // g_game->Tick();
        }
    }

    // g_game.reset();

    // Closes the COM library on the current thread, 
    // unloads all DLLs loaded by the thread, 
    // frees any other resources that the thread maintains, 
    // and forces all RPC connections on the thread to close.
    CoUninitialize();

    return hwnd;
}