#include <ApplicationLayer/Initialization.h>

// CPU speed can be read from the system registry
DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	// open the key where the proc speed is hidden:
	auto errorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	if (errorCode == ERROR_SUCCESS)
	{
		// query the key:
		RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}
	return dwMHz;
}

bool CheckCPUSpeed(DWORD minCpuSpeed)
{
	if (ReadCPUSpeed() < minCpuSpeed)
	{
		throw std::exception("CheckCPUSpeed failure: CPU is too slow for this game.");
	}

	return true;
}