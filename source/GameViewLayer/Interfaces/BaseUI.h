#pragma once

#include <pch.h>

#include "IScreenElement.h"

// This class is a group of user interface controls.
// It can be modal or modeless.

//
// class BaseUI									- Chapter 10, page 286  
//
// This was factored to create a common class that
// implements some of the IScreenElement class common
// to modal/modeless dialogs
//
class BaseUI : public IScreenElement
{
protected:
	i32	m_PosX, m_PosY;
	i32	m_Width, m_Height;
	std::optional<i32> m_Result;
	bool m_bIsVisible;

public:
	inline BaseUI() : 
		m_PosX(0),
		m_PosY(0),
		m_Width(100),
		m_Height(100),
		m_bIsVisible(true)
	{}

	inline void OnUpdate(u32 dt) override {};
	inline HRESULT OnLostDevice() override { return S_OK; }
	inline bool IsVisible() const override { return m_bIsVisible; }
	inline void SetVisible(bool visible) override { m_bIsVisible = visible; }
};