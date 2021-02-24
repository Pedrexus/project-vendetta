#include "ProcessManager.h"

#include <const.h>
#include <helpers.h>

ProcessManager::ProcessManager(void) :
    m_processList({})
{}

ProcessManager::~ProcessManager(void)
{
    m_processList.clear();
}

void ProcessManager::UpdateProcess(std::shared_ptr<Process> currentProcess, milliseconds dt)
{   
    // process is uninitialized, so initialize it
    if (currentProcess->IsInitialized())
        currentProcess->OnInit();

    // give the process an update tick if it's running
    if (currentProcess->IsRunning())
        currentProcess->OnUpdate(dt);

    // check to see if the process is dead
    if (currentProcess->IsDead())
    {
        // run the appropriate exit function
        switch (currentProcess->GetState())
        {
            case Process::State::SUCCEEDED:
            {
                currentProcess->OnSuccess();
                auto child = currentProcess->RemoveChild();
                if (child)
                    AttachProcess(child);
                else
                    LOG("Process", "Process chain completed successfully");  // only counts if the whole chain completed
                break;
            }

            case Process::State::FAILED:
            {
                currentProcess->OnFail();

                LOG("Process", "Process chain failed");
                break;
            }

            case Process::State::ABORTED:
            {
                currentProcess->OnAbort();

                LOG("Process", "Process chain was aborted");
                break;
            }
        }

        // remove the process and destroy it
        m_processList.remove(currentProcess);
    }
}

//---------------------------------------------------------------------------------------------------------------------
// The process update tick.  Called every logic tick.  This function returns the number of process chains that 
// succeeded in the upper 32 bits and the number of process chains that failed or were aborted in the lower 32 bits.
//---------------------------------------------------------------------------------------------------------------------
void ProcessManager::UpdateAllProcesses(milliseconds dt)
{
    std::for_each(
        LOOP_EXECUTION_POLICY,
        m_processList.begin(),
        m_processList.end(),
        [this, &dt] (std::shared_ptr<Process> p) { UpdateProcess(p, dt); }
    ); 
}

std::weak_ptr<Process> ProcessManager::AttachProcess(std::shared_ptr<Process> process)
{
    m_processList.push_front(process);
    return std::weak_ptr(process);
}

void ProcessManager::AbortAllProcesses(bool immediate)
{
    for (auto& process : m_processList)
    {
        if (process->IsAlive())
        {
            process->Abort();
            if (immediate)
            {
                process->OnAbort();
                m_processList.remove(process);
            }
        }
    }
}
