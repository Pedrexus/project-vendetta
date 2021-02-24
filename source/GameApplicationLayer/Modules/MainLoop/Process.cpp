#include "Process.h"

Process::Process(void) :
	m_state(Process::State::UNINITIALIZED),
	m_child(nullptr)
{}

Process::~Process(void)
{
	if (m_child)
		m_child->OnAbort();
}

//---------------------------------------------------------------------------------------------------------------------
// Removes the child from this process.  This releases ownership of the child to the caller and completely removes it
// from the process chain.
//---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<Process> Process::RemoveChild(void)
{
	if (m_child)
	{
		std::shared_ptr<Process> child = m_child; // this keeps the child from getting destroyed when we clear it
		m_child.reset();
		return child;
	}

	return nullptr;
}