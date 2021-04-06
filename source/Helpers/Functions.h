#pragma once

#include <pch.h>
#include <types.h>
#include <const.h>
#include <macros.h>

namespace Convert
{
	std::string StringToLower(std::string s);
	std::wstring ANSIToUNICODE(const char* source, const unsigned int length);

	std::string ws2s(std::wstring const& wstr);
	const wchar_t* s2ws(const char* str);

	std::string int2hex(i64 number);
}

namespace Mem
{
	inline std::string as_kb(u64 size) { return std::to_string(size >> 10) + "Kb"; }
	inline std::string as_mb(u64 size) { return std::to_string(size >> 20) + "Mb"; }
	inline std::string as_gb(u64 size) { return std::to_string(size >> 30) + "Gb"; }
	inline std::string as_tb(u64 size) { return std::to_string(size >> 40) + "Tb"; }
}

// windows specific
namespace MachineRead
{
	// CPU speed can be read from the system registry
	const u32 CPUSpeed();
	const u64 FreePhysicalRAM();
	const u64 FreeVirtualMemory();
	const u64 FreeStorage();
	const std::wstring CreateSaveGameDirectory(const std::wstring gameAppDirectory);
}

// since these are templates, we keep them in .h
namespace Factory
{
	template<class T>
	std::function<T* (void)> NewPointer()
	{
		return [] (void) -> T* { return NEW T(); };
	}
}


namespace COM
{

	inline void Initialize()
	{
		// Initializes COM: a standard under which different pieces of software interact
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr))
			throw std::exception("Failed at initializing COM with HRESULT " + hr);
	}

	inline void Shutdown()
	{
		// Closes the COM library on the current thread, 
		// unloads all DLLs loaded by the thread, 
		// frees any other resources that the thread maintains, 
		// and forces all RPC connections on the thread to close.
		CoUninitialize();
	}
}



void SetMemoryChecks();
void ClearFile(const char* filename);
bool WildcardMatch(const char* pat, const char* str);