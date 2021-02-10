#include "../Initialization.h"

#include <const.h>
#include <macros.h>

// Most games need a bit of free secondary storage space for saving games
bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	const auto drive = _getdrive();
	struct _diskfree_t diskfree;

	const auto errorCode = _getdiskfree(drive, &diskfree);

	if (errorCode != ERROR_SUCCESS)
	{
		LOG_ERROR("_getdiskfree returned error code " + errorCode);
		return false;
	}

	const auto availableStorage = diskfree.avail_clusters * diskfree.sectors_per_cluster * diskfree.bytes_per_sector;
	LOG_INFO("Free physical storage: " + std::to_string(availableStorage / MEGABYTE) + "Mb");

	if (availableStorage < diskSpaceNeeded)
	{
		LOG_ERROR("Not enough physical storage.");
		return false;
	}

	return true;
}