#include "../Initialization.h"

// Most games need a bit of free secondary storage space for saving games
bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	if (MachineRead::FreeStorage() > diskSpaceNeeded)
		return true;

	LOG_ERROR("Not enough physical storage.");
	return false;
}