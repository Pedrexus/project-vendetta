#include "Game.h"

Game* Game::instance = nullptr;
std::mutex Game::mutex = {};

bool Game::Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, INT nCmdShow)
{
	// if (!GameApp::Initialize(hInstance, lpCmdLine, hWnd, nCmdShow))
	// 		LOG_FATAL("Failed to initialized Game Application Layer");

	m_humanView = NEW HumanView(hInstance);
	m_humanView->Initialize();

	LOG_INFO("Game initialized");

	return true;
}

void Game::Shutdown()
{
	// GameApp::Shutdown();
	SAFE_DELETE(m_humanView);

	LOG_INFO("Game shutdown");
}

void Game::DispatchGameMessage(MSG& msg)
{
	m_humanView->OnMessage(msg);
}

void Game::OnUpdate(milliseconds dt)
{
	m_humanView->OnUpdate(dt);
}

void Game::Run()
{
	MSG msg = {};
	m_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		m_timer.Tick();

		auto isWindowMessage = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
		if (isWindowMessage)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DispatchGameMessage(msg);
			OnUpdate(m_timer.GetDeltaMilliseconds());
		}
	}
}
