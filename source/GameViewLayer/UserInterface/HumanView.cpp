#include "HumanView.h"

#include <const.h>
#include <DirectXMath.h>

#include "../InputHandlers/Controller.h"
#include "../GraphicsEngines/DX12/DX12Engine.h"

HumanView::HumanView(HINSTANCE hInstance) :
	m_GameState(Initializing),
	m_KeyboardHandler(nullptr)
{
	m_windowManager = std::make_shared<WindowManager>(hInstance);
	m_PointerHandler = std::make_shared<Controller>();
	m_graphicsEngine = std::make_shared<DX12Engine>();
}

HumanView::~HumanView()
{
}

void HumanView::Initialize()
{
	m_windowManager->Initialize();
	m_graphicsEngine->Initialize();
	OnResize();
}

void HumanView::OnUpdate(milliseconds dt)
{
	m_graphicsEngine->SetCameraPosition(m_PointerHandler->GetCameraPosition());
	m_graphicsEngine->OnUpdate(dt);
	m_graphicsEngine->OnDraw();
}

void HumanView::OnMessage(MSG msg)
{
	switch (msg.message)
	{
		case WM_KEYDOWN:
			// TODO: Add console
			if (m_KeyboardHandler)
				m_KeyboardHandler->OnKeyDown(LOBYTE(msg.wParam));
			break;

		case WM_KEYUP:
			// TODO: Add console
			if (m_KeyboardHandler)
				m_KeyboardHandler->OnKeyUp(LOBYTE(msg.wParam));
			break;

		case WM_MOUSEMOVE:
			if (m_PointerHandler)
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				Point p(LOWORD(msg.lParam), HIWORD(msg.lParam));
				m_PointerHandler->OnPointerMove(p, m_PointerRadius);
			}
			break;

		case WM_LBUTTONDOWN:
			if (m_PointerHandler)
			{
				SetCapture(msg.hwnd);
				Point p(LOWORD(msg.lParam), HIWORD(msg.lParam));
				m_PointerHandler->OnPointerButtonDown(p, m_PointerRadius, MK_LBUTTON);
			}
			break;

		case WM_LBUTTONUP:
			if (m_PointerHandler)
			{
				ReleaseCapture();
				Point p(LOWORD(msg.lParam), HIWORD(msg.lParam));
				m_PointerHandler->OnPointerButtonUp(p, m_PointerRadius, MK_LBUTTON);
			}
			break;

		case WM_RBUTTONDOWN:
			if (m_PointerHandler)
			{
				SetCapture(msg.hwnd);
				Point p(LOWORD(msg.lParam), HIWORD(msg.lParam));
				m_PointerHandler->OnPointerButtonDown(p, m_PointerRadius, MK_RBUTTON);
			}
			break;

		case WM_RBUTTONUP:
			if (m_PointerHandler)
			{
				ReleaseCapture();
				Point p(LOWORD(msg.lParam), HIWORD(msg.lParam));
				m_PointerHandler->OnPointerButtonUp(p, m_PointerRadius, MK_RBUTTON);
			}
			break;

		case WM_CHAR: // TODO: add console
		default:
			break;
	}
}


