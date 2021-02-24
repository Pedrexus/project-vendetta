#pragma once

#include <pch.h>
#include <macros.h>

#include "ProcessManager.h"

class Process
{
	friend class ProcessManager;

	// the process state
	enum class State
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
	virtual void OnInit(void) { m_state = State::RUNNING; }  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void OnUpdate(milliseconds dt) = 0;  // called every frame
	virtual void OnSuccess(void) { }  // called if the process succeeds (see below)
	virtual void OnFail(void) { }  // called if the process fails (see below)
	virtual void OnAbort(void) { }  // called if the process is aborted (see below)

public:
	// construction
	Process(void);
	virtual ~Process(void);

	// Functions for ending the process.
	inline void Succeed(void) { if (IsAliveOrError()) SetState(State::SUCCEEDED); };
	inline void Fail(void) { if (IsAliveOrError()) SetState(State::FAILED); };

	// pause
	inline void Pause(void)
	{
		if (IsRunning())
			SetState(State::PAUSED);
		else
			LOG_WARNING("Attempting to pause a process that isn't running");
	}

	inline void Unpause(void)
	{
		if (IsPaused())
			SetState(State::RUNNING);
		else
			LOG_WARNING("Attempting to unpause a process that isn't paused");
	}

	inline void Abort(void)
	{
		if(!IsDead())
			SetState(State::ABORTED);
		else
			LOG_WARNING("Attempting to abort a dead process");
	}

	// accessors
	State GetState(void) const { return m_state; }
	inline bool IsInitialized(void) const { return m_state != State::UNINITIALIZED; }
	inline bool IsRunning(void) const { return m_state == State::RUNNING; }
	inline bool IsAlive(void) const { return (m_state == State::RUNNING || m_state == State::PAUSED); }
	inline bool IsDead(void) const { return (m_state == State::SUCCEEDED || m_state == State::FAILED || m_state == State::ABORTED); }
	inline bool IsRemoved(void) const { return (m_state == State::REMOVED); }
	inline bool IsPaused(void) const { return m_state == State::PAUSED; }

	inline bool IsAliveOrError(void) const { 
		if (IsAlive())
			return true;

		LOG_ERROR("Process is not alive");
		return true;
	}

	// child functions
	inline std::shared_ptr<Process> AttachChild(std::shared_ptr<Process> child) { return m_child ? m_child->AttachChild(m_child) : m_child = child; }
	std::shared_ptr<Process> RemoveChild(void);  // releases ownership of the child
	inline std::shared_ptr<Process> PeekChild(void) { return m_child; }  // doesn't release ownership of the child
	
};