#pragma once

#include <pch.h>
#include "../BaseUI.h"

enum MessageBox_Questions
{
	QUESTION_WHERES_THE_CD,
	QUESTION_QUIT_GAME,
};

class UIMessageBox : public BaseUI
{
protected:
	CDXUTDialog m_UI; // DirectX dialog
	i32 m_ButtonId;

public:
	UIMessageBox(std::wstring msg, std::wstring title, i32 buttonFlags = MB_OK);
	~UIMessageBox() = default;

	// IScreenElement Implementation
	HRESULT OnRestore() override;
	HRESULT OnRender(f64 time, f32 elapsedTime) override;
	inline i32 GetZOrder() const override { return 99; }
	inline void SetZOrder(int const zOrder) override {}

	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) override;
	static void CALLBACK OnGUIEvent(UINT nEvent, i32 nControlID, CDXUTControl* pControl, void* pUserContext);
	static i32 Ask(MessageBox_Questions question);

};
