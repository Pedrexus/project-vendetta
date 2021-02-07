#include <ApplicationLayer/Initialization.h>

// Most games need a bit of free secondary storage space for saving games
bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	const auto drive = _getdrive();
	struct _diskfree_t diskfree;

	const auto errorCode = _getdiskfree(drive, &diskfree);

	if (errorCode != ERROR_SUCCESS)
	{
		throw std::exception("CheckStorage Failure: _getdiskfree returned error code %d", errorCode);
	}

	const auto bytes_per_cluster = diskfree.sectors_per_cluster * diskfree.bytes_per_sector;
	const auto neededClusters = diskSpaceNeeded / bytes_per_cluster;

	if (diskfree.avail_clusters < neededClusters)
	{
		// if you get here you don't have enough disk space!
		throw std::exception("CheckStorage Failure: Not enough physical storage.");
	}

	return true;
}