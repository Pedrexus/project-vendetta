#include <ApplicationLayer/MainLoop/ProcessManager.h>

//---------------------------------------------------------------------------------------------------------------------
// The process update tick.  Called every logic tick.  This function returns the number of process chains that 
// succeeded in the upper 32 bits and the number of process chains that failed or were aborted in the lower 32 bits.
//---------------------------------------------------------------------------------------------------------------------
UINT ProcessManager::UpdateProcesses(ULONG deltaMilliseconds)
{
    unsigned short int successCount = 0;
    unsigned short int failCount = 0;

    auto it = m_processList.begin();
    while (it != m_processList.end())
    {
        // grab the next process
        auto &currentProcess = (*it);

        // save the iterator and increment the old one in case we need to remove this process from the list
        ProcessList::iterator thisIt = it;
        ++it;

        // process is uninitialized, so initialize it
        if (currentProcess->IsInitialized())
            currentProcess->OnInit();

        // give the process an update tick if it's running
        if (currentProcess->IsRunning())
            currentProcess->OnUpdate(deltaMilliseconds);

        // check to see if the process is dead
        if (currentProcess->IsDead())
        {
            // run the appropriate exit function
            switch (currentProcess->GetState())
            {
                case Process::SUCCEEDED:
                {
                    currentProcess->OnSuccess();
                    auto child = currentProcess->RemoveChild();
                    if (child)
                        AttachProcess(child);
                    else
                        ++successCount;  // only counts if the whole chain completed
                    break;
                }

                case Process::FAILED:
                {
                    currentProcess->OnFail();
                    ++failCount;
                    break;
                }

                case Process::ABORTED:
                {
                    currentProcess->OnAbort();
                    ++failCount;
                    break;
                }
            }

            // remove the process and destroy it
            m_processList.erase(thisIt);
        }
    }

    return ((successCount << 16) | failCount);  // a single number, two values
}