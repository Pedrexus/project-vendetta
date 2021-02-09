#include <ApplicationLayer/Initialization.h>

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);
	LOG_INFO("Available RAM: " + std::to_string(status.ullTotalPhys / MEGABYTE) + "Mb. Available Virtual Memory: " + std::to_string(status.ullAvailVirtual / MEGABYTE) + "Mb");

	if (status.ullTotalPhys < physicalRAMNeeded)
	{
		LOG_FATAL("Not enough physical memory.");
		return false;
	}

	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		LOG_FATAL("Not enough virtual memory.");
		return false;
	}

	return true;
}