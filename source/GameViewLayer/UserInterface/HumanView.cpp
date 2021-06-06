#include "HumanView.h"

#include <DirectXMath.h>

#include "../InputHandlers/Controller.h"
#include "../GraphicsEngines/DX12/DX12Engine.h"

#include <DirectXTK12/Keyboard.h>
#include <DirectXTK12/Mouse.h>


HumanView::HumanView(HINSTANCE hInstance) :
	m_GameState(Initializing)
{
	m_windowManager = std::make_unique<WindowManager>(hInstance);
	m_graphicsEngine = std::make_unique<DX12Engine>();
}

HumanView::~HumanView()
{
}

void HumanView::Initialize()
{
	m_windowManager->Initialize();

	auto [width, height] = m_windowManager->GetDimensions();
	m_graphicsEngine->Initialize(m_windowManager->GetMainWnd(), width, height);
}

void HumanView::OnUpdate(milliseconds dt)
{
	static auto engine = dynamic_cast<DX12Engine*>(m_graphicsEngine.get());

	// TODO: put this on specific methods and use names explicitly async
	std::thread(&DX12Engine::OnUpdate, engine, dt).detach();
	
	// m_graphicsEngine->SetCameraPosition(m_PointerHandler->GetCameraPosition());
	// m_graphicsEngine->OnUpdate(dt);
	
	// not thread safe, messes with the commands to be executed by the queue;
	// not necessary btw
	m_graphicsEngine->OnDraw();
}
