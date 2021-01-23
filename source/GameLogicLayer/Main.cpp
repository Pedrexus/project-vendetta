#include <windows.h>

#include <DirectXMath.h>

// remember: set linker -> system -> subsystem = windows
INT WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ INT nShowCmd
)
{
    if (!DirectX::XMVerifyCPUSupport())
    {
        // creates a pop up windows message box
        MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
        return -1;
    }

    MessageBox(nullptr, TEXT("Everything worked."), TEXT("Success"), MB_OK);
    return 0;
}
