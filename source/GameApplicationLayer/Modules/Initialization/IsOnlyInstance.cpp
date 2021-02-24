#include "../Initialization.h"

#include <macros.h>

// Find the window. If active, set and return false
// Only one game instance may have this mutex at a time...
bool IsOnlyInstance(LPCWSTR gameTitle) // LPCTSTR = const char*
{
	/* A mutex is a process synchronization mechanism
		- common to any multitasking operating system
		- guarantees one mutex is created with the identifier gameTitle for all processes running
	*/
	HANDLE mutexHandle = CreateMutex(NULL, TRUE, gameTitle);

	// GetLastError: calling thread's last-error code value
	// QUESTION: if last error is success, this is the only instance(?)
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND windowHandle = FindWindow(gameTitle, NULL);
		if (windowHandle)
		{
			LOG_WARNING("An instance of your game was already running.");

			ShowWindow(windowHandle, SW_SHOWNORMAL);
			SetFocus(windowHandle);
			SetForegroundWindow(windowHandle);
			SetActiveWindow(windowHandle);
			return false;
		}
	}

	return true;
}