#include <ApplicationLayer/Initialization.h>

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);

	if (status.ullTotalPhys < physicalRAMNeeded)
	{
		// you don't have enough physical memory.
		throw std::exception("CheckMemory Failure: Not enough physical memory.");
	}

	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		// you don't have enough virtual memory available. 
		throw std::exception("CheckMemory Failure: Not enough virtual memory.");
	}

	return true;
}