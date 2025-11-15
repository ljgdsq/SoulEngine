#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <windowsx.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "D3DApp.h"

using Microsoft::WRL::ComPtr;

class MyApp : public D3DApp
{
public:
    MyApp(HINSTANCE hInstance)
        : D3DApp(hInstance)
    {
        mMainWndCaption = L"SoulEngine DX12Demo";
        mClientWidth = 1280;
        mClientHeight = 720;
    }
    ~MyApp() override = default;
};

static void SetupConsole()
{
    if (GetConsoleWindow())
        return;
    if (AllocConsole())
    {
        FILE *fp = nullptr;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
    }
}


#include "target_path.h"

void InitD3D12();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
    
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

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting DX12App");

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


    MyApp app(hInstance);
    if (app.Initialize())
    {
        return app.Run();
    }

     spdlog::info("DX12App finished frames");

    float exitDelay = 2.0f; // seconds
    spdlog::info("Exiting in {} seconds...", exitDelay);
    Sleep(static_cast<DWORD>(exitDelay * 1000));

    return 0;
}

