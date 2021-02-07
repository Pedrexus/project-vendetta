#include <ApplicationLayer/Initialization.h>

void SetMemoryChecks()
{
	// Memory Checks - Chapter 21, page 834
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/crtsetdbgflag
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// SLOWDOWN: 
	// set this flag to keep memory blocks around
	// tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;

	// HEAVY SLOWDOWN: 
	// perform memory check for each alloc/dealloc
	// tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;	

	// force a leak check just before program exit
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF; // bitwise inclusive OR operator

	_CrtSetDbgFlag(tmpDbgFlag);
}