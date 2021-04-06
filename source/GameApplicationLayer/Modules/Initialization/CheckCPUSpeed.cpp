#include "../Initialization.h"
#include <Helpers/Functions.h>

bool CheckCPUSpeed(DWORD minCpuSpeed)
{	
	auto CPUSpeed = MachineRead::CPUSpeed();

	if (CPUSpeed > minCpuSpeed)
		return true;

	LOG_FATAL("CPU is too slow for this game.");
	return false;
}