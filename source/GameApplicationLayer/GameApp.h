#pragma once

#include <pch.h>
#include <const.h>
#include <macros.h>
#include <helpers.h>

#include "Modules/Initialization.h"
#include "Modules/ResourceCache.h"
#include "Modules/EventSystem.h"
#include "Modules/ProcessSystem.h"
#include "Modules/LuaScriptingSystem.h"
#include "Modules/AudioSystem.h"
#include "Modules/GraphicsSystem.h"

#define Accessor(type, member) \
	static inline std::shared_ptr<type> Get##type() { return std::shared_ptr<type>(Get()->member); };

#define Accessor2(name, type, member) \
	static inline std::shared_ptr<type> Get##name() { return std::shared_ptr<type>(Get()->member); };


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
#pragma region Singleton
private:
	inline static GameApp* instance;
	inline static std::mutex mutex;

protected:
	GameApp();
	~GameApp() = default;

public:
	GameApp(GameApp& other) = delete; // Singletons should not be cloneable.
	void operator=(const GameApp&) = delete; // Singletons should not be assignable.

	static inline void Destroy() { SAFE_DELETE(instance); };
	static inline GameApp* Get()
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (instance == nullptr)
		{
			LOG("Lua", "GameApp instantiated");
			instance = NEW GameApp();
		}

		return instance;
	}
#pragma endregion

private:
	TCHAR m_saveGameDirectory[MAX_PATH];

	ResourceCache* m_ResCache;
	ProcessManager* m_ProcessManager;
	XAudioManager* m_AudioManager;
	DX12Engine* m_GraphicsEngine;
	Timer m_timer;

	std::map<std::wstring, UINT> m_hotkeys;
	std::map<std::wstring, std::wstring> m_textResource; // strings

public:
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
	virtual bool Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, INT nCmdShow);
	virtual LRESULT Shutdown();

	inline void AttachProcess(std::shared_ptr<Process> pProcess) { if (m_ProcessManager) m_ProcessManager->AttachProcess(pProcess); }

// event handlers
public:
	inline void OnSuspending() { m_timer.Stop(); }
	inline void OnResuming() { m_timer.Start(); } // Unpause

	inline bool IsReady() { return m_GraphicsEngine->IsReady(); } // TODO: should check if Init has been called
	inline bool IsSuspended() { return m_timer.IsPaused(); }

public:
	virtual bool Run(); // main loop
	virtual void OnLoopIteration(); // game loop iteration
protected:
	virtual inline void OnUpdate() {};

public:
	// static accessors
	Accessor(ResourceCache, m_ResCache);
	Accessor(XAudioManager, m_AudioManager);

	// accessors
	inline DX12Engine* GetWindow() { return m_GraphicsEngine; }

protected:
	UINT MapCharToKeycode(const char pHotKey);
	bool LoadStrings(std::string language);
	void MeasureFrameStats();
};