#pragma once

#include <pch.h>
#include <macros.h>
#include <const.h>

extern bool IsOnlyInstance(LPCWSTR gameTitle);
extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool CheckCPUSpeed(DWORD minCpuSpeed);

// TODO: make a device-window manager class
extern bool RegisterWindowClass(HINSTANCE hInstance);
extern RECT CreateWindowRectangle(INT screenWidth, INT screenHeight);
extern const TCHAR* GetSaveGameDirectory(const TCHAR* gameAppDirectory);
