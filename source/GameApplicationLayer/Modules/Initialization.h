#pragma once

#include <pch.h>

#include <macros.h>

extern bool IsOnlyInstance(LPCWSTR gameTitle);
extern bool CheckStorage(const DWORDLONG diskSpaceNeeded);
extern bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
extern bool CheckCPUSpeed(DWORD minCpuSpeed);
extern const TCHAR* GetSaveGameDirectory(const TCHAR* gameAppDirectory);
