#pragma once
#include "d3dUtil.h"
#include "GameTimer.h"
// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{

protected:
    D3DApp(HINSTANCE hInstance);
    D3DApp(const D3DApp& rhs) = delete;
    D3DApp& operator=(const D3DApp& rhs) = delete;
    virtual ~D3DApp();

public:
    static D3DApp* GetApp();
    virtual bool Initialize();
    int Run();
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    bool InitMainWindow();
    bool InitDirect3D();

protected:
   static D3DApp* mApp;

    HINSTANCE mhAppInst = nullptr; // application instance handle
    HWND      mhMainWnd = nullptr; // main window handle



    std::wstring mMainWndCaption = L"d3d App";
    int mClientWidth = 800;
    int mClientHeight = 600;
};
