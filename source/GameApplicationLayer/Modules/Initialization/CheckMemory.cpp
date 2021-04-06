#include "../Initialization.h"
#include <Helpers/Functions.h>

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{

	// Check for enough free memory.
	if (MachineRead::FreeVirtualMemory() < virtualRAMNeeded)
	{
		LOG_FATAL("Not enough virtual memory.");
		return false;
	}

	if (MachineRead::FreePhysicalRAM() < physicalRAMNeeded)
	{
		LOG_FATAL("Not enough physical memory.");
		return false;
	}

	

	return true;
}