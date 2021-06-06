#pragma once

#include <pch.h>
#include <types.h>

#include "../Interfaces/IGameView.h"
#include "../Interfaces/IScreenElement.h"
#include "../Interfaces/InputHandlers.h"
#include "../Interfaces/Enums.h"
#include "../OutputHandlers/WindowManager.h"
#include "../GraphicsEngines/IGraphicsEngine.h"

class HumanView : public IGameView
{
	friend class GameApp;

protected:
	GameState m_GameState;
	GameViewId m_ViewId = Human;
	// ActorId m_ActorId;

	bool m_isRunningFullSpeed;
	
	// Interface sensitive objects
	std::unique_ptr<WindowManager> m_windowManager;
	std::unique_ptr<IGraphicsEngine> m_graphicsEngine;

public:
	HumanView(HINSTANCE hInstance);
	virtual ~HumanView();

	GameViewType GetType() override { return Human; }
	GameViewId GetId() const override { return m_ViewId; }

	virtual void Initialize();
	inline bool IsReady() { return m_windowManager->IsReady() && m_graphicsEngine->IsReady(); }

	virtual void OnUpdate(milliseconds dt);
	virtual inline void OnResize(u32 width = NULL, u32 height = NULL) 
	{
		if (width == NULL || height == NULL)
			std::tie(width, height) = m_windowManager->GetDimensions();
		else
			m_windowManager->OnResize(width, height);
		m_graphicsEngine->OnResize(width, height);
	}

	inline WindowManager* GetWindow() { return m_windowManager.get(); };
};