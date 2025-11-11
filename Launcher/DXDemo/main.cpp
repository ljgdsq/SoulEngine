#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <windowsx.h>
#include <string>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "MatrixUtil.h"
#include "D3DHeader.h"

ID3D11Device *g_device = nullptr;
ID3D11DeviceContext *g_immediateContext = nullptr;
ID3D11RenderTargetView *g_renderTargetView = nullptr;
IDXGISwapChain *g_swapChain = nullptr;

void CleanUp();
bool Display(float delta);

bool InitD3d(HINSTANCE hInstance, int w, int h, ID3D11RenderTargetView **targetView,
             ID3D11DeviceContext **immediateContext, IDXGISwapChain **swapChain, ID3D11Device **device);

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
void TestMatrixUtil();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

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

    if (!InitD3d(hInstance, 800, 600, &g_renderTargetView, &g_immediateContext, &g_swapChain, &g_device))
    {
        spdlog::error("Failed to initialize D3D11");
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
        Display(0.16f);
        // Update and render your engine here
        // SoulEngine::Update();
        // SoulEngine::Render();

        frameCount++;
    }

    spdlog::info("DXDemo finished, ran {} frames", frameCount);

    CleanUp();
    // Cleanup resources
    // SoulEngine::Shutdown();

    return 0;
}

void TestMatrixUtil()
{
    using namespace DirectX;
    using namespace MatrixUtil;
    XMMATRIX mat = XMMatrixIdentity();
    std::cout << "Identity Matrix:\n"
              << mat << std::endl;

    XMVECTOR vec = {1.0f, 2.0f, 3.0f, 4.0f};
    std::cout << "Vector:\n"
              << vec << std::endl;

    XMMATRIX scale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    std::cout << "Scaling Matrix:\n"
              << scale << std::endl;

    XMMATRIX translation = XMMatrixTranslation(5.0f, 5.0f, 5.0f);
    std::cout << "Translation Matrix:\n"
              << translation << std::endl;

    float angle = XMConvertToRadians(30.0f);

    XMMATRIX rotation = XMMatrixRotationY(angle);
    std::cout << "Rotation Matrix :\n"
              << rotation << std::endl;

    XMMATRIX transform = mat * scale * rotation * translation;
    std::cout << "Combined Transform Matrix:\n"
              << transform << std::endl;
}

bool InitD3d(HINSTANCE hInstance, int w, int h, ID3D11RenderTargetView **targetView,
             ID3D11DeviceContext **immediateContext, IDXGISwapChain **swapChain, ID3D11Device **device)
{
    // 1.创建swapchain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = w;
    sd.BufferDesc.Height = h;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    sd.OutputWindow = MainWindowHandle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    // 2.创建device
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, swapChain, device, NULL, immediateContext);
    if (FAILED(hr))
    {
        MessageBoxA(MainWindowHandle, "D3D11CreateDeviceAndSwapChain Failed!", "Error", MB_OK);
        return false;
    }

    // 3.创建RenderTarget
    hr = 0;
    ID3D11Texture2D *pBackBuffer = nullptr;
    hr = (*swapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer);
    if (FAILED(hr))
    {
        MessageBoxA(MainWindowHandle, "GetBuffer Failed!", "Error", MB_OK);
        return false;
    }

    hr = (*device)->CreateRenderTargetView(pBackBuffer, nullptr, targetView);
    pBackBuffer->Release();

    if (FAILED(hr))
    {
        MessageBoxA(MainWindowHandle, "CreateRenderTargetView Failed!", "Error", MB_OK);
        return false;
    }

    (*immediateContext)->OMSetRenderTargets(1, targetView, nullptr);

    // 4.创建viewport

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)w;
    vp.Height = (FLOAT)h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    (*immediateContext)->RSSetViewports(1, &vp);

    return true;
}

void CleanUp()
{
    if (g_renderTargetView)
        g_renderTargetView->Release();
    if (g_immediateContext)
        g_immediateContext->Release();
    if (g_swapChain)
        g_swapChain->Release();
    if (g_device)
        g_device->Release();
}

bool Display(float delta)
{
    if (g_device)
    {
        float ClearColor[4] = {0.2f, 0.4f, 0.3f, 1};
        g_immediateContext->ClearRenderTargetView(g_renderTargetView, ClearColor);

        g_swapChain->Present(0, 0);
    }
    return true;
}