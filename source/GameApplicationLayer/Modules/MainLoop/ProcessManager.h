#pragma once

#include <pch.h>
#include <types.h>

// avoid circular dependency
class Process;

#include "Process.h"

class ProcessManager
{
	typedef std::list<std::shared_ptr<Process>> ProcessList;

	ProcessList m_processList;

public:
	// construction
	ProcessManager(void);
	~ProcessManager(void);

	// interface
	void UpdateProcess(std::shared_ptr<Process> currentProcess, milliseconds dt);
	void UpdateAllProcesses(milliseconds dt);  // updates all attached processes
	std::weak_ptr<Process> AttachProcess(std::shared_ptr<Process> process);  // attaches a process to the process mgr
	void AbortAllProcesses(bool immediate);

	// accessors
	inline u64 GetProcessCount(void) const { return m_processList.size(); }
};