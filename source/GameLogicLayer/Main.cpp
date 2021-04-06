
#include "Game.h"
#include <Helpers/Functions.h>


// remember: set linker -> system -> subsystem = windows
// windows entrypoint (wWinfMain for Unicode applications)
INT WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ INT nShowCmd
)
{
    // always the first
    SetMemoryChecks();

    // always the second
    Logger::Init("logging.xml");

    auto game = Game::Get();

    if (!game->Initialize(hInstance, lpCmdLine, 0, nShowCmd))
        return FALSE; // Fix memory leaks if we hit this branch. // TODO: print an error.

    game->Run();

    game->Shutdown();

    // MessageBox(nullptr, TEXT("Everything worked."), TEXT("Success"), MB_OK);

    Game::Destroy();
    Logger::Destroy();

    return 0;
}
