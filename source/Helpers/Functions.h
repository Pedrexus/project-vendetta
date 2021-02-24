#pragma once

#include <pch.h>
#include <types.h>
#include <const.h>
#include <macros.h>

namespace Convert
{
	std::string StringToLower(std::string s);
	std::wstring ANSIToUNICODE(const char* source, const unsigned int length);
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

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception("DirectX has failed");
		}
	}
}


void SetMemoryChecks();
void ClearFile(const char* filename);
bool WildcardMatch(const char* pat, const char* str);