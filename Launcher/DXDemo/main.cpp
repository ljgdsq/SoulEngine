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

ID3D11VertexShader* g_vertexShader = nullptr;
ID3D11PixelShader* g_pixelShader = nullptr;


bool SetupShader();
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

#include "target_path.h"

void LogAdaptors();
void LogOutputs(IDXGIAdapter* adapter);
void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

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
    spdlog::info("Starting SoulEngine DXDemo (WinMain)");

    spdlog::info("Current directory: {}", buf);

    auto verifyResult = DirectX::XMVerifyCPUSupport();
    if (!verifyResult)
    {
        spdlog::error("DirectX Math CPU support verification failed: {}", static_cast<int>(verifyResult));
    }else{
        spdlog::info("DirectX Math CPU support verified successfully");
    }
    // Here you can add your engine initialization code
    // SoulEngine::Initialize();
    LogAdaptors();
    // Main loop example
    bool running = true;
    int frameCount = 0;

    if (!InitWindowsApp(hInstance, nShowCmd))
    {
        spdlog::error("Failed to initialize Windows application");
        return -1;
    }

    // flush
    fflush(stdout);
    fflush(stderr);


    spdlog::flush_on(spdlog::level::info);

    // flush spdlog
    spdlog::default_logger()->flush();


    if (!InitD3d(hInstance, 800, 600, &g_renderTargetView, &g_immediateContext, &g_swapChain, &g_device))
    {
        spdlog::error("Failed to initialize D3D11");
        return -1;
    }

    if (!SetupShader())
    {
        spdlog::error("Failed to setup shaders");
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

UINT createDeviceFlags = 0;
// vscode 会卡死
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    spdlog::info("D3D11_CREATE_DEVICE_DEBUG flag set");
    #endif
    
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, swapChain, device, NULL, immediateContext);
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

    if (g_vertexShader)
        g_vertexShader->Release();
    if (g_pixelShader)
        g_pixelShader->Release();
}

bool Display(float delta)
{
    if (g_device)
    {
        float ClearColor[4] = {0.02f, 0.124f, 0.3f, 1.0f};
        g_immediateContext->ClearRenderTargetView(g_renderTargetView, ClearColor);

        g_immediateContext->VSSetShader(g_vertexShader, nullptr, 0);
        g_immediateContext->PSSetShader(g_pixelShader, nullptr, 0);

        g_immediateContext->Draw(3, 0);


        g_swapChain->Present(0, 0);
    }
    return true;
}

struct Vertex{
    DirectX::XMFLOAT3 position;
};


bool SetupShader(){

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    ID3DBlob* vsBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"Triangle.hlsl",nullptr,nullptr,"VSMain","vs_5_0",shaderFlags,0,&vsBlob,nullptr);
    if(FAILED(hr)){
        spdlog::error("Failed to compile vertex shader");
        return false;
    }

    hr = g_device->CreateVertexShader(vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),nullptr,&g_vertexShader);
    if(FAILED(hr)){
        spdlog::error("Failed to create vertex shader");
        vsBlob->Release();
        return false;
    }

    hr = D3DCompileFromFile(L"Triangle.hlsl",nullptr,nullptr,"PSMain","ps_5_0",shaderFlags,0,&vsBlob,nullptr);
    if(FAILED(hr)){
        spdlog::error("Failed to compile pixel shader");
        return false;
    }
    hr = g_device->CreatePixelShader(vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),nullptr,&g_pixelShader);
    if(FAILED(hr)){
        spdlog::error("Failed to create pixel shader");
        vsBlob->Release();
        return false;
    }


    D3D11_INPUT_ELEMENT_DESC layout[] ={
        {
            "POSITION",
            0,
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    UINT numElements = ARRAYSIZE(layout);
    ID3D11InputLayout *inputLayout = nullptr;

    hr = g_device->CreateInputLayout(layout,numElements,vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),&inputLayout);
    if(FAILED(hr)){
        spdlog::error("Failed to create input layout");
        vsBlob->Release();
        return false;
    }

    g_immediateContext->IASetInputLayout(inputLayout);

    inputLayout->Release();
    vsBlob->Release();

    Vertex vertices[] = {
        {DirectX::XMFLOAT3(0.0f,0.5f,0.0f)},
        {DirectX::XMFLOAT3(0.5f,-0.5f,0.0f)},
        {DirectX::XMFLOAT3(-0.5f,-0.5f,0.0f)}
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd,sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData,sizeof(initData));
        initData.pSysMem = vertices;

    ID3D11Buffer *vertexBuffer = nullptr;

    hr=g_device->CreateBuffer(&bd,&initData,&vertexBuffer);
    if(FAILED(hr)){
        spdlog::error("Failed to create vertex buffer");
        return false;
    }
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    g_immediateContext->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
    g_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return true;

}

#include <vector>

void LogAdaptors()
{
    UINT i=0;
    IDXGIAdapter *adapter = nullptr;
    std::vector<IDXGIAdapter*> adapters;
    IDXGIFactory *factory = nullptr;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if(FAILED(hr)){
        spdlog::error("Failed to create DXGI Factory");
        return;
    }

    while(factory->EnumAdapters(i,&adapter) != DXGI_ERROR_NOT_FOUND){
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring wdesc(desc.Description);
        std::string sdesc(wdesc.begin(),wdesc.end());

        spdlog::info("Adapter {}: {}",i,sdesc);
        adapters.push_back(adapter);
        i++;

        LogOutputs(adapter);

        spdlog::info("--------------------");
    }


    for(auto& adp : adapters){
        adp->Release();
    }
    factory->Release();

}

void LogOutputs(IDXGIAdapter* adapter){

    UINT i = 0;
    IDXGIOutput *output = nullptr;
    while(adapter->EnumOutputs(i,&output) != DXGI_ERROR_NOT_FOUND){
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring wdesc(desc.DeviceName);
        std::string sdesc(wdesc.begin(),wdesc.end());

        spdlog::info("Output {}: {}",i,sdesc);
        
        LogOutputDisplayModes(output, DXGI_FORMAT_R8G8B8A8_UNORM);
        output->Release();
        i++;
    }

}

void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format){
    UINT count = 0;
    UINT flags = 0;

    output->GetDisplayModeList(format,flags,&count,nullptr);
    std::vector<DXGI_MODE_DESC> modeDescs(count);
    output->GetDisplayModeList(format,flags,&count,modeDescs.data());

    for(const auto& mode : modeDescs){
        spdlog::info("Mode: {}x{} @ {}/{} Hz, Format: {}",mode.Width,mode.Height,mode.RefreshRate.Numerator,mode.RefreshRate.Denominator,static_cast<int>(mode.Format));
    }
}