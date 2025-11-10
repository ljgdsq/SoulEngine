#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <windowsx.h>

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

HWND MainWindowHandle = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        std::string msg = "Middle mouse button clicked at (" +
                          std::to_string(xPos) + ", " + std::to_string(yPos) + ")";
        MessageBoxA(hwnd, msg.c_str(), "Mouse Click", MB_OK);
    }
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

bool InitWindowsApp(HINSTANCE hInstance, int nShowCmd)
{
    // Register window class
    const char CLASS_NAME[] = "SoulEngineDXDemoWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc))
    {
        spdlog::error("Failed to register window class");
        return false;
    }

    // Create the window
    MainWindowHandle = CreateWindowEx(
        0,
        CLASS_NAME,
        "SoulEngine DXDemo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!MainWindowHandle)
    {
        spdlog::error("Failed to create window");
        return false;
    }

    ShowWindow(MainWindowHandle, nShowCmd);
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    // 这些参数不使用
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    // Show console so you can see output
    SetupConsole();

    // Initialize logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting SoulEngine DXDemo (WinMain)");

    std::cout << "Hello from SoulEngine DXDemo!" << std::endl;

    // Here you can add your engine initialization code
    // SoulEngine::Initialize();

    // Main loop example
    bool running = true;
    int frameCount = 0;

    if (!InitWindowsApp(hInstance, nShowCmd))
    {
        spdlog::error("Failed to initialize Windows application");
        return -1;
    }

    while (running)
    {
        // Process Windows messages
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update and render your engine here
        // SoulEngine::Update();
        // SoulEngine::Render();

        frameCount++;
    }

    spdlog::info("DXDemo finished, ran {} frames", frameCount);

    // Cleanup resources
    // SoulEngine::Shutdown();

    return 0;
}
