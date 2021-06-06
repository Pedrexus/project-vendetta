#pragma once

#include <pch.h>
#include <types.h>

#include "Enums.h"

typedef u32 GameViewId;

class IGameView
{
public:
	virtual GameViewType GetType() = 0;
	virtual GameViewId GetId() const = 0;

	virtual void OnUpdate(milliseconds dt) = 0;
};