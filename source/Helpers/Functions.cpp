#include <pch.h>
#include <types.h>
#include <macros.h>

namespace Convert
{
	std::string StringToLower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[] (unsigned char c) { return std::tolower(c); } // correct
		);
		return s;
	}
	std::wstring ANSIToUNICODE(const char* source, const unsigned int length)
	{
		if (source == nullptr || length < 1) // Fail
			return std::wstring();

		int len = MultiByteToWideChar(CP_ACP, 0, source, length, 0, 0) - 1;

		std::wstring r(len, '\0');

		MultiByteToWideChar(CP_ACP, 0, source, length, &r[0], len);

		return r;
	}
}

namespace Mem
{
	std::string as_kb(u64 size) { return std::to_string(size >> 10) + "Kb"; }
	std::string as_mb(u64 size) { return std::to_string(size >> 20) + "Mb"; }
	std::string as_gb(u64 size) { return std::to_string(size >> 30) + "Gb"; }
	std::string as_tb(u64 size) { return std::to_string(size >> 40) + "Tb"; }
}

// windows specific
namespace MachineRead
{
	// CPU speed can be read from the system registry
	const u32 CPUSpeed()
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
			RegQueryValueEx(hKey, L"~MHz", NULL, &type, (LPBYTE) &dwMHz, &BufSize);
		}

		LOG_INFO("CPU Speed: " + std::to_string(dwMHz) + "MHz");
		return dwMHz;
	}

	const MEMORYSTATUSEX GetMemoryStatus()
	{
		MEMORYSTATUSEX status{};
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status;
	}

	const u64 FreePhysicalRAM()
	{
		auto status = GetMemoryStatus();
		LOG_INFO("Available Physical RAM: " + Mem::as_gb(status.ullAvailPhys));
		return status.ullAvailPhys;
	}

	const u64 FreeVirtualMemory()
	{
		// FIXME: this may not return valid results
		auto status = GetMemoryStatus();
		LOG_INFO("Available Virtual Memory: " + Mem::as_gb(status.ullAvailVirtual));
		return status.ullAvailVirtual;
	}

	const u64 FreeStorage()
	{
		// Check for enough free disk space on the current disk.
		const auto drive = _getdrive();
		struct _diskfree_t diskfree;

		const auto errorCode = _getdiskfree(drive, &diskfree);

		if (errorCode != ERROR_SUCCESS)
			LOG_ERROR("_getdiskfree returned error code " + errorCode);

		u64 availableStorage = static_cast<u64>(diskfree.avail_clusters) * diskfree.sectors_per_cluster * diskfree.bytes_per_sector;
		LOG_INFO("Free physical storage: " + Mem::as_mb(availableStorage));

		return availableStorage;
	}

	const std::wstring CreateSaveGameDirectory(const std::wstring gameAppDirectory)
	{
		// MAX_PATH: windows maximum length for a path
		PWSTR saveGameDirectory[MAX_PATH];

		// get the Saved Games folder from the default user
		if (FAILED(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, NULL, saveGameDirectory)))
			LOG_FATAL("Unable to get Saved Games folder path");

		if (!PathAppend(*saveGameDirectory, gameAppDirectory.c_str()))
			LOG_FATAL("Path append failed");

		// if the dir doesn't exist
		if (GetFileAttributes(*saveGameDirectory) == 0xffffffff)
		{
			if (!CreateDirectory(*saveGameDirectory, NULL))
				LOG_FATAL("Error creating save game directory");
		}

		return *saveGameDirectory;
	}

}

void SetMemoryChecks()
{
	// Memory Checks - Chapter 21, page 834
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/crtsetdbgflag
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// SLOWDOWN: 
	// set this flag to keep memory blocks around
	// tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;

	// HEAVY SLOWDOWN: 
	// perform memory check for each alloc/dealloc
	// tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;	

	// force a leak check just before program exit
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF; // bitwise inclusive OR operator

	_CrtSetDbgFlag(tmpDbgFlag);
}

void ClearFile(const char* filename)
{
	std::ofstream ofs;
	ofs.open(filename, std::ofstream::out | std::ofstream::trunc);
	ofs.close();
}

// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to 
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos. 
bool WildcardMatch(const char* pat, const char* str)
{
	int i, star;

new_segment:

	star = 0;
	if (*pat == '*')
	{
		star = 1;
		do { pat++; }
		while (*pat == '*'); /* enddo */
	} /* endif */

test_match:

	for (i = 0; pat[i] && (pat[i] != '*'); i++)
	{
		//if (mapCaseTable[str[i]] != mapCaseTable[pat[i]]) {
		if (str[i] != pat[i])
		{
			if (!str[i]) return 0;
			if ((pat[i] == '?') && (str[i] != '.')) continue;
			if (!star) return 0;
			str++;
			goto test_match;
		}
	}
	if (pat[i] == '*')
	{
		str += i;
		pat += i;
		goto new_segment;
	}
	if (!str[i]) return 1;
	if (i && pat[i - 1] == '*') return 1;
	if (!star) return 0;
	str++;
	goto test_match;
}