#pragma once

#include <pch.h>

// Class Console
//
// This class offers a method for entering script commands at runtime.
// Activate it by pressing the Tilde (~) key during runtime, and type
// "exit" or "~" and pressing enter.
class Console
{
	bool m_bActive;

	std::queue<std::string> m_DisplayStrings;

	RECT m_ConsoleOutputRect;	//Where results get shown
	RECT m_ConsoleInputRect;	//Where input is entered

	// DirectX::PackedVector::XMCOLOR m_InputColor;
	// DirectX::PackedVector::XMCOLOR m_OutputColor;

	std::string m_CurrentOutputString;	//What's the current output string?
	std::string m_CurrentInputString;	//What's the current input string?

	int m_ConsoleInputSize;	//Height of the input console window

	int m_CursorBlinkTimer;	//Countdown to toggle cursor blink state
	bool m_bCursorOn;	//Is the cursor currently displayed?

	bool m_bShiftDown;	//Is the shift button down?
	bool m_bCapsLockDown;	//Is the caps lock button down?

	//If this is true, we have a string to execute on our next update cycle.  
	//We have to do this because otherwise the user could interrupt in the midst
	//of an ::Update() call in another system.  This causes problems.
	bool m_bExecuteStringOnUpdate;

public:
	Console(void) {};
	~Console(void) {};

	void AddDisplayText(const std::string& newText) {};
	void SetDisplayText(const std::string& newText) {};

	void SetActive(const bool bIsActive) { m_bActive = bIsActive; }
	bool IsActive(void) const { return m_bActive; }

	void HandleKeyboardInput(const unsigned int keyVal, const unsigned int oemKeyVal, const bool bKeyDown) {};

	void Update(const int deltaMilliseconds) {};

	void Render() {};
};