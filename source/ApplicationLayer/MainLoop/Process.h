#pragma once

#include <pch.h>
#include <ApplicationLayer/MainLoop/ProcessManager.h>

class Process
{
	friend class ProcessManager;

	// the process state
	enum State
	{
		// Processes that are neither dead nor alive
		UNINITIALIZED = 0,  // created but not running
		REMOVED,  // removed from the process list but not destroyed; this can happen when a process that is already running is parented to another process

		// Living processes
		RUNNING,  // initialized and running
		PAUSED,  // initialized but paused

		// Dead processes
		SUCCEEDED,  // completed successfully
		FAILED,  // failed to complete
		ABORTED,  // aborted; may not have started
	};
	 
	State m_state;  // the current state of the process
	std::shared_ptr<Process> m_child;  // the child process, if any

	void SetState(State newState) { m_state = newState; }

protected:
	// interface; these functions should be overridden by the subclass as needed
	virtual void OnInit(void) { m_state = RUNNING; }  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void OnUpdate(unsigned long deltaMilliseconds) = 0;  // called every frame
	virtual void OnSuccess(void) { }  // called if the process succeeds (see below)
	virtual void OnFail(void) { }  // called if the process fails (see below)
	virtual void OnAbort(void) { }  // called if the process is aborted (see below)

public:
	// construction
	Process(void);
	virtual ~Process(void);

	// Functions for ending the process.
	inline void Succeed(void);
	inline void Fail(void);

	// pause
	inline void Pause(void);
	inline void UnPause(void);

	// accessors
	State GetState(void) const { return m_state; }
	inline bool IsInitialized(void) const { return m_state != UNINITIALIZED; }
	inline bool IsRunning(void) const { return m_state == RUNNING; }
	inline bool IsAlive(void) const { return (m_state == RUNNING || m_state == PAUSED); }
	inline bool IsDead(void) const { return (m_state == SUCCEEDED || m_state == FAILED || m_state == ABORTED); }
	inline bool IsRemoved(void) const { return (m_state == REMOVED); }
	inline bool IsPaused(void) const { return m_state == PAUSED; }

	// child functions
	inline void AttachChild(std::shared_ptr<Process> child);
	std::shared_ptr<Process> RemoveChild(void);  // releases ownership of the child
	inline std::shared_ptr<Process> PeekChild(void) { return m_child; }  // doesn't release ownership of the child
	
};