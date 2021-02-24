#include "HumanView.h"

#include <const.h>

HRESULT HumanView::OnRestore()
{
	for (auto& element : m_ScreenElements)
		DX::ThrowIfFailed(element->OnRestore());
	return S_OK;
}

HRESULT HumanView::OnLostDevice()
{
	for (auto& element : m_ScreenElements)
		DX::ThrowIfFailed(element->OnLostDevice());
	return S_OK;
}

void HumanView::OnRender(f64 time, f32 elapsedTime)
{
	m_currTick = timeGetTime();
	if (m_currTick == m_lastDraw)
		return;

	// It is time to draw ?
	if (m_runFullSpeed || ((m_currTick - m_lastDraw) > SCREEN_REFRESH_RATE))
	{
		if (g_GameApp->m_Renderer->PreRender())
		{
			m_ScreenElements.sort([] (const auto& lhs, const auto& rhs) -> const bool { return *lhs < *rhs; });

			for (auto& element : m_ScreenElements)
				if (element->IsVisible())
					element->OnRender(time, elapsedTime);

			RenderText();

			// Let the console render.
			m_Console.Render();

			// record the last successful paint
			m_lastDraw = m_currTick;
		}

		g_GameApp->m_Renderer->PostRender();
	}
}

void HumanView::OnUpdate(const u32 dt)
{
	m_pProcessManager->UpdateAllProcesses(dt);
	m_Console.Update(dt);

	// Some screen elements need to update every frame.
	// For example, particle effects that do not affect the Game Logic Layer 
	// and are only know by the Game View Layer
	std::for_each(
		LOOP_EXECUTION_POLICY,
		m_ScreenElements.begin(),
		m_ScreenElements.end(),
		[this, &dt] (std::shared_ptr<IScreenElement> e) { e->OnUpdate(dt); }
	);
}

LRESULT HumanView::OnMsgProc(AppMsg msg)
{
	// Iterate through the screen layers first
	// In reverse order since we'll send input messages to the 
	// screen on top

	for (auto i = m_ScreenElements.rbegin(); i != m_ScreenElements.rend(); ++i)
	{
		auto& element = *i;
		if (element->IsVisible() && element->OnMsgProc(msg))
			return TRUE;
	}

	LRESULT result = FALSE;
	switch (msg.m_uMsg)
	{
		case WM_KEYDOWN:
			if (m_Console.IsActive())
				noop; // Let the console eat this.
			else if (m_KeyboardHandler)
				result = m_KeyboardHandler->OnKeyDown(static_cast<const u8>(msg.m_wParam));
			break;

		case WM_KEYUP:
			if (m_Console.IsActive())
				noop; // Let the console eat this.
			else if (m_KeyboardHandler)
				result = m_KeyboardHandler->OnKeyUp(static_cast<const u8>(msg.m_wParam));
			break;

		case WM_MOUSEMOVE:
			if (m_PointerHandler)
			{
				Point p(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam));
				result = m_PointerHandler->OnPointerMove(p, m_PointerRadius);
			}
			break;

		case WM_LBUTTONDOWN:
			if (m_PointerHandler)
			{
				SetCapture(msg.m_hWnd);
				Point p(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam));
				result = m_PointerHandler->OnPointerButtonDown(p, m_PointerRadius, Pointer::Left);
			}
			break;

		case WM_LBUTTONUP:
			if (m_PointerHandler)
			{
				SetCapture(nullptr);
				Point p(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam));
				result = m_PointerHandler->OnPointerButtonUp(p, m_PointerRadius, Pointer::Left);
			}
			break;

		case WM_RBUTTONDOWN:
			if (m_PointerHandler)
			{
				SetCapture(msg.m_hWnd);
				Point p(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam));
				result = m_PointerHandler->OnPointerButtonDown(p, m_PointerRadius, Pointer::Right);
			}
			break;

		case WM_RBUTTONUP:
			if (m_PointerHandler)
			{
				SetCapture(nullptr);
				Point p(LOWORD(msg.m_lParam), HIWORD(msg.m_lParam));
				result = m_PointerHandler->OnPointerButtonDown(p, m_PointerRadius, Pointer::Right);
			}
			break;
		case WM_CHAR:
			if (m_Console.IsActive())
			{
				const u32 oemScan = int(msg.m_lParam & (0xff << 16)) >> 16;
				m_Console.HandleKeyboardInput(msg.m_wParam, MapVirtualKey(oemScan, 1), true);
			}
			else
			{
				// See if it was the console key.
				if (('~' == msg.m_wParam) || ('`' == msg.m_wParam))
					m_Console.SetActive(true);
			}
			break;
		default:
			return FALSE;
	}

	return result;
}
