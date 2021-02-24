#pragma once

#include <pch.h>
#include <types.h>

#include <ApplicationLayer/Modules/Actors.h>

#include "Others.h"

typedef u32 GameViewId;

class IGameView
{
public:
	virtual HRESULT OnRestore() = 0; // responsible for re-creating anything that might be lost while the game is running
	virtual void OnRender(f64 time, f32 elapsedTime) = 0;
	virtual HRESULT OnLostDevice() = 0;
	virtual GameViewType GetType() = 0;
	virtual GameViewId GetId() const = 0;
	virtual void OnAttach(GameViewId vid, ActorId aid) = 0;

	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) = 0;
	virtual void OnUpdate(u64 dt) = 0;

	virtual ~IGameView() {};
};