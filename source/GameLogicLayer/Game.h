#pragma once

#include <macros.h>

#include <GameApplicationLayer/GameApp.h>
#include <GameViewLayer/GameView.h>
#include <Helpers/Timer/Timer.h>

typedef std::list<std::shared_ptr<IGameView>> GameViewList;

class Game : public GameApp
{
#pragma region Singleton
private:
	static Game* instance;
	static std::mutex mutex;

protected:
	Game() : m_humanView(nullptr) {};
	~Game() = default;

public:
	Game(Game& other) = delete; // Singletons should not be cloneable.
	void operator=(const Game&) = delete; // Singletons should not be assignable.

	static inline void Destroy() { SAFE_DELETE(instance); };
	static inline Game* Get()
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (instance == nullptr)
		{
			LOG_INFO("Game instantiated");
			instance = NEW Game();
		}

		return instance;
	}
#pragma endregion

	Timer m_timer;
	HumanView* m_humanView;

public:
	bool Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, INT nCmdShow);
	void Shutdown();

	inline bool IsReady() { return m_humanView->IsReady(); }
	inline std::shared_ptr<WindowManager> GetWindow() { return m_humanView->GetWindow(); }
	void OnResize(u32 width = NULL, u32 height = NULL) { return m_humanView->OnResize(width, height); };

public:
	void Run();

protected:
	void DispatchGameMessage(MSG& msg);
	void OnUpdate(milliseconds dt);
};