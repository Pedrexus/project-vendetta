#pragma once

#include "GraphicsEngines/DX12Engine.h"

// TODO: rename to graphics engine
class GraphicsManager
{
	DX12Engine m_GraphicsEngine;

public:
	GraphicsManager(HINSTANCE hInstance) : m_GraphicsEngine(DX12Engine(hInstance)) {}

	inline void Initialize() { m_GraphicsEngine.OnInit(); }
};

