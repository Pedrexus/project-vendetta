// edit Properties -> VC++ Directories -> Include Directories
#include "GameApp.h"

#include <helpers.h>

GameApp::GameApp() :
	m_saveGameDirectory(L""),
	m_ResCache(nullptr),
	m_textResource({}),
	m_hotkeys({})
{
	// only the game (GameApp subclass) calls the constructor
};

bool GameApp::Initialize(
	HINSTANCE hInstance,
	LPWSTR lpCmdLine,
	HWND hWnd,
	INT nCmdShow
)
{
	DX::COM::Initialize();

	// Check for existing instance of the same window
	// TODO: create a splash screen to help minimize this problem
	if (!IsOnlyInstance(GAME_TITLE))
		return false;

	// We don't need a mouse cursor by default, let the game turn it on
	SetCursor(NULL);

	if (!CheckStorage(DISK_SPACE))
		return false;

	if (!CheckMemory(PHYSICAL_RAM, VIRTUAL_RAM))
		return false;

	if (!CheckCPUSpeed(CPU_SPEED))
		return false;

	// resets and starts the game clock
	m_timer.Reset();

	// TODO: development editor resource cache initialization - chapter 22
	IResourceFile* zipFile = NEW ResourceZipFile(RESOURCES_ZIPFILE);
	m_ResCache = NEW ResourceCache(RESOURCES_SIZE, zipFile);

	if (!m_ResCache->Init())
		return false;

	m_AudioManager = NEW XAudioManager();
	if (!m_AudioManager->Initialize(hWnd))
		return false;

	// Note - register these in order from least specific to most specific! They get pushed onto a list.
	// RegisterLoader is discussed in Chapter 5, page 142
	m_ResCache->RegisterLoader(std::shared_ptr<IResourceLoader>{ NEW WaveResourceLoader() });
	m_ResCache->RegisterLoader(std::shared_ptr<IResourceLoader>{ NEW OGGResourceLoader() });
	// m_ResCache->RegisterLoader(CreateDDSResourceLoader());
	// m_ResCache->RegisterLoader(CreateJPGResourceLoader());
	m_ResCache->RegisterLoader(std::shared_ptr<IResourceLoader>{ NEW XMLResourceLoader() });
	// m_ResCache->RegisterLoader(CreateSdkMeshResourceLoader());
	// m_ResCache->RegisterLoader(CreateScriptResourceLoader());

	// Load strings with the XML Resource Loader
	LoadStrings(GAME_LANGUAGE);

	// TODO: event manager and event registering - Chapter 11
	auto m_EventManager = EventManager::Get();

	// TODO: Lua script manager initialization - Chapter 12
	LuaStateManager::Get()->Init();

	m_GraphicsEngine = NEW DX12Engine(hInstance);
	m_GraphicsEngine->OnInit();

	// TODO: create game logic & view - Chapter 21

	// copy the string ptr
	_tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(SAVE_GAME_DIR));

	// now that all the major systems are initialized, preload resources - Chapter 8

	return true;
}

// Shutdown sequence occurs in reverse order from initialization
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
	// VDestroyNetworkEventForwarder();
	// SAFE_DELETE(m_pBaseSocketManager);
	SAFE_DELETE(m_GraphicsEngine);
	EventManager::Destroy();
	// BaseScriptComponent::UnregisterScriptFunctions();
	// ScriptExports::Unregister();
	LuaStateManager::Destroy();
	SAFE_DELETE(m_ResCache);
	DX::COM::Shutdown();

	return 0;
}

// Starts the game
bool GameApp::Run()
{
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			m_timer.Tick();

			if (IsSuspended())
				Sleep(100);
			else
				this->OnLoopIteration();
		}
	}

	return msg.wParam;
}

void GameApp::OnLoopIteration()
{
	this->MeasureFrameStats();
	// this->OnUpdate();

	m_GraphicsEngine->Draw();
}


UINT GameApp::MapCharToKeycode(const char pHotKey)
{
	UINT keycode = 0;
	if (pHotKey >= '0' && pHotKey <= '9')
		keycode = 0x30 + pHotKey - '0';
	else if (pHotKey >= 'A' && pHotKey <= 'Z')
		keycode = 0x41 + pHotKey - 'A';
	else
		LOG_ERROR("Platform specific hotkey " + std::to_string(pHotKey) + " is not defined");
	return keycode;
}

bool GameApp::LoadStrings(std::string language)
{
	auto languageFile = "Strings\\" + language + ".xml";
	auto root = m_ResCache->GetData<ResourceData::XML>(languageFile)->GetRoot();

	if (!root)
	{
		LOG_ERROR("Strings are missing.");
		return false;
	}

	// Loop through each child element and load the component
	auto counter = 0;
	for (auto node = root->FirstChildElement(); node; node = node->NextSiblingElement())
	{
		const char* id = node->Attribute("id");
		const char* value = node->Attribute("value");
		const char* hotkey = node->Attribute("hotkey");
		if (id && value)
		{
			auto wideKey = Convert::ANSIToUNICODE(id, 64);
			auto wideText = Convert::ANSIToUNICODE(value, KILOBYTE);
			// m_textResource.insert({ wideKey, wideText });
			m_textResource[wideKey] = wideText;

			if (hotkey)
				m_hotkeys[wideKey] = MapCharToKeycode(*hotkey);

			counter++;
		}
		else
		{
			LOG_WARNING("Malformed XML element at " + languageFile);
		}
	}

	LOG_INFO(std::to_string(counter) + " strings loaded from " + languageFile);
	return true;
}

// Code computes the average frames per second, and also the 
// average time it takes to render one frame.  These stats 
// are appended to the window caption bar.
void GameApp::MeasureFrameStats()
{
	static u32 frameCount = 0;
	static u64 totalFrames = 0;
	static f64 timeElapsed = 0;

	frameCount++;

	// Compute averages over one second period.
	auto totalTime = m_timer.GetTotalTime();
	if ((totalTime - timeElapsed) >= 1.0f)
	{
		auto fps = (f32) frameCount; // fps = frameCnt / 1
		auto mspf = 1000.0f / fps; // milliseconds per frame
		auto windowText = fmt::format(L"{} fps: {} mspf: {:.6f} total: {}", WINDOW_TITLE_NAME, fps, mspf, totalFrames);

		SetWindowText(m_GraphicsEngine->GetMainWnd(), windowText.c_str());

		// Reset for next average.
		totalFrames += frameCount;
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}
