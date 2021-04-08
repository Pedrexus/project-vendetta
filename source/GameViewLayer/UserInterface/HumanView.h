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
	u32 m_PointerRadius = 1; // we assume we are on a mouse enabled machine - if this were a tablet we should detect and change the value in the constructor.
	std::shared_ptr<IPointerHandler> m_PointerHandler;
	std::shared_ptr<IKeyboardHandler> m_KeyboardHandler;
	std::shared_ptr<WindowManager> m_windowManager;
	std::shared_ptr<IGraphicsEngine> m_graphicsEngine;

public:
	HumanView(HINSTANCE hInstance);
	virtual ~HumanView();

	GameViewType GetType() override { return Human; }
	GameViewId GetId() const override { return m_ViewId; }

	virtual void Initialize();
	inline bool IsReady() { return m_windowManager->IsReady() && m_graphicsEngine->IsReady(); }

	virtual void OnUpdate(milliseconds dt);
	virtual void OnMessage(MSG msg);
	virtual inline void OnResize(u32 width = NULL, u32 height = NULL) 
	{
		if (width == NULL || height == NULL)
			std::tie(width, height) = m_windowManager->GetDimensions();
		else
			m_windowManager->OnResize(width, height);
		m_graphicsEngine->OnResize(width, height);
	}

	inline std::shared_ptr<WindowManager> GetWindow() { return m_windowManager; };
};