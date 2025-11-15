#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <windowsx.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "d3dx12.h"


D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView();
D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();

// Allocate a console window for a WIN32 subsystem app so logs/printf are visible
static void SetupConsole()
{
    // If already has a console (debugger), skip
    if (GetConsoleWindow())
        return;
    if (AllocConsole())
    {
        FILE *fp = nullptr;
        // Redirect stdout/stderr to the new console
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
    }
}





#include "target_path.h"





int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SetCurrentDirectoryA(TARGET_OUTPUT_PATH);
    char buf[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buf);

    // 这些参数不使用
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    FILE *fp = nullptr;
    AllocConsole();
    // freopen("CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    // Show console so you can see output
    SetupConsole();

    // Initialize logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting SoulEngine DX12Demo (WinMain)");

    spdlog::info("Current directory: {}", buf);

    auto verifyResult = DirectX::XMVerifyCPUSupport();
    if (!verifyResult)
    {
        spdlog::error("DirectX Math CPU support verification failed: {}", static_cast<int>(verifyResult));
    }
    else
    {
        spdlog::info("DirectX Math CPU support verified successfully");
    }


    


    return 0;
}
