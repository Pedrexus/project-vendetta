#pragma once

#include <pch.h>
#include <types.h>

#include <ApplicationLayer/Modules/Process.h>
#include <ApplicationLayer/GameApp.h>

#include "../IGameView.h"
#include "../IScreenElement.h"
#include "../InputHandlers.h"

#include "Console.h"

class HumanView : public IGameView
{
	friend class GameApp;

protected:
	GameViewId m_ViewId;
	ActorId m_ActorId;

	ProcessManager* m_pProcessManager; // strictly for things like button animations, etc.

	u64 m_currTick; // time right now
	u64 m_lastDraw;	// last time the game rendered
	bool m_runFullSpeed; // set to true if you want to run full speed

	// Interface sensitive objects
	u32 m_PointerRadius;
	std::shared_ptr<IPointerHandler> m_PointerHandler;
	std::shared_ptr<IKeyboardHandler> m_KeyboardHandler;

	std::list<std::shared_ptr<IScreenElement>> m_ScreenElements; // a game screen entity

	Console m_Console;

	BaseGameState m_BaseGameState;	// current game state

	virtual void RenderText() {};

public:
	HumanView(std::shared_ptr<IRenderer> renderer);
	virtual ~HumanView();

	virtual GameViewType GetType() { return GameViewType::Human; }
	virtual GameViewId GetId() const { return m_ViewId; }

	// Implement the IGameView interface, except for the VOnRender() method, which is renderer specific
	virtual HRESULT OnRestore();
	virtual HRESULT OnLostDevice();
	virtual void OnRender(f64 time, f32 elapsedTime);
	virtual void OnUpdate(const u32 dt);
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg);
	virtual void OnAttach(GameViewId vid, ActorId aid)
	{
		m_ViewId = vid;
		m_ActorId = aid;
	}

	// Virtual methods to control the layering of interface elements
	virtual void PushElement(std::shared_ptr<IScreenElement> element);
	virtual void RemoveElement(std::shared_ptr<IScreenElement> element);

	void TogglePause(bool active);

	// Audio
	bool InitAudio();
	inline ProcessManager* GetProcessManager() { return m_pProcessManager; }

	//Camera adjustments.
	virtual void SetCameraOffset(const Vec4& camOffset);

	// Added post press
	std::shared_ptr<ScreenElementScene> m_pScene;
	std::shared_ptr<CameraNode> m_pCamera;

	void HandleGameState(BaseGameState newState);

	inline Console& GetConsole(void) { return m_Console; }

	// Added post press - this helps the network system attach views to the right actor.
	inline virtual void SetControlledActor(ActorId actorId) { m_ActorId = actorId; }

	bool LoadGame(tinyxml2::XMLElement* levelData);
protected:
	inline virtual void LoadGameDelegate(tinyxml2::XMLElement* pLevelData) { PushElement(m_pScene); }

public:
	// Event delegates
	void PlaySoundDelegate(IEventDataPtr pEventData);
	void GameStateDelegate(IEventDataPtr pEventData);

private:
	void RegisterAllDelegates(void);
	void RemoveAllDelegates(void);
	
};