#include "D3DApp.h"
D3DApp* D3DApp::mApp = nullptr;


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}


D3DApp::D3DApp(HINSTANCE hInstance)
{
    mApp = this;
}

D3DApp::~D3DApp()
{
}

D3DApp* D3DApp::GetApp()
{
    return mApp;
}

bool D3DApp::Initialize()
{
    if(!InitMainWindow())
        return false;
    if(!InitDirect3D())
        return false;

    return true;
}

int D3DApp::Run()
{
    while (true)
    {
        Sleep(100);
    }
    
    return 0;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = mhAppInst;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = L"D3DAppWndClass";

    if(!RegisterClass(&wc))
    {
        MessageBox(0, L"RegisterClass Failed.", 0, 0);
        return false;
    }

    RECT R = {0, 0, mClientWidth, mClientHeight};
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width  = R.right - R.left;
    int height = R.bottom - R.top;
    mhMainWnd = CreateWindow(L"D3DAppWndClass", mMainWndCaption.c_str(), 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
    if(!mhMainWnd)
    {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(mhMainWnd, SW_SHOW);
    UpdateWindow(mhMainWnd);

    return true;
}


bool D3DApp::InitDirect3D()
{
    return true;
}