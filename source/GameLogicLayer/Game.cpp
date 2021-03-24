// project headers
#include <helpers.h>

#include <GameApplicationLayer/GameApp.h>

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
    Logger::Init("logging.xml");

    auto game = GameApp::Get();

    if (!game->Initialize(hInstance, lpCmdLine, 0, nShowCmd))
    {
        // LOG_FATAL("Failed intializing GameApp");
        return FALSE; // Fix memory leaks if we hit this branch. // TODO: print an error.
    }

    // game loop
    if (!game->Run())
         return FALSE;

    // shutdown
    game->Shutdown();

    MessageBox(nullptr, TEXT("Everything worked."), TEXT("Success"), MB_OK);

    GameApp::Destroy();
    Logger::Destroy();

    return 0;
}
