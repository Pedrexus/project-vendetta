#include "../Initialization.h"

const TCHAR* GetSaveGameDirectory(const TCHAR* gameAppDirectory)
{
	// MAX_PATH: windows maximum length for a path

	PWSTR saveGameDirectory[MAX_PATH];

	// get the Saved Games folder from the default user
	if (FAILED(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_DEFAULT, NULL, saveGameDirectory)))
		LOG_FATAL("Unable to get Saved Games folder path");

	if (!PathAppend(*saveGameDirectory, gameAppDirectory))
		LOG_FATAL("Path append failed");

	// if the dir doesn't exist
	if (GetFileAttributes(*saveGameDirectory) == 0xffffffff)
	{
		if (!CreateDirectory(*saveGameDirectory, NULL))
			LOG_FATAL("Error creating save game directory");
	}

	return *saveGameDirectory;
}