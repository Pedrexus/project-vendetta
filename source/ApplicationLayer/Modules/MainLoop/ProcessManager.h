#pragma once

#include <pch.h>
#include <ApplicationLayer/MainLoop/Process.h>


class ProcessManager
{
	typedef std::list<std::shared_ptr<Process>> ProcessList;

	ProcessList m_processList;

	void ClearAllProcesses(void);  // should only be called by the destructor

public:
	// construction
	~ProcessManager(void);

	// interface
	UINT UpdateProcesses(ULONG deltaMilliseconds);  // updates all attached processes
	std::weak_ptr<Process> AttachProcess(std::shared_ptr<Process> process);  // attaches a process to the process mgr
	void AbortAllProcesses(bool immediate);

	// accessors
	inline UINT GetProcessCount(void) const { return m_processList.size(); }
};