// edit Properties -> VC++ Directories -> Include Directories
#include "GameApp.h"

#include <Helpers/Functions.h>

bool GameApp::Initialize(
	HINSTANCE hInstance,
	LPWSTR lpCmdLine,
	HWND hWnd,
	INT nCmdShow
)
{
	COM::Initialize();

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

	// TODO: create game logic & view - Chapter 21

	// copy the string ptr
	_tcscpy_s(m_saveGameDirectory, GetSaveGameDirectory(SAVE_GAME_DIR));

	// now that all the major systems are initialized, preload resources - Chapter 8

	return true;
}

// Shutdown sequence occurs in reverse order from initialization
void GameApp::Shutdown()
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
	EventManager::Destroy();
	// BaseScriptComponent::UnregisterScriptFunctions();
	// ScriptExports::Unregister();
	LuaStateManager::Destroy();
	SAFE_DELETE(m_ResCache);
	COM::Shutdown();
}

void GameApp::Run()
{
	MSG msg = {};
	m_timer.Reset();

	while (msg.message != WM_QUIT)
		ProcessMessage(msg);
}

void GameApp::ProcessMessage(MSG& msg)
{
	auto isWindowMessage = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
	if (isWindowMessage)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
		TickAndDispatchGameMessage(msg);
}

void GameApp::TickAndDispatchGameMessage(MSG& msg)
{
	m_timer.Tick();

	if (IsSuspended())
		Sleep(100);
	else
	{
		DispatchGameMessage(msg);
		OnUpdate(m_timer.GetDeltaMilliseconds());
	}
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
