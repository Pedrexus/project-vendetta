// project headers
#include <ApplicationLayer/GameApp.h>

// remember: set linker -> system -> subsystem = windows
// windows entrypoint (wWinfMain for Unicode applications)
INT WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ INT nShowCmd
)
{
    // always the first: set memory check flags.
    SetMemoryChecks();

    // always the second: initialize logging system.
    // FUTURE: Logger::Init("logging.xml");

    if (!DirectX::XMVerifyCPUSupport())
    {
        // creates a pop up windows message box
        MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
        return -1;
    }

    (void)GameApp();

    if (g_GameApp->Initialize(hInstance, lpCmdLine, 0, nShowCmd))
    {
        // Fix memory leaks if we hit this branch.
        // TODO: print an error.
        return FALSE;
    }

    MessageBox(nullptr, TEXT("Everything worked."), TEXT("Success"), MB_OK);

    g_GameApp->Shutdown();

    return 0;
}