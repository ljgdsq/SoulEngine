
#include "d3dApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"


struct VertexPosColor
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};


const D3D11_INPUT_ELEMENT_DESC VertexPosColor::inputLayout[2] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


class GameApp : public D3DApp
{
public:
    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp() override = default;

    bool Init() override;
    void OnResize() override;
    void UpdateScene(float dt) override;
    void DrawScene() override;

    bool InitResources();
    bool InitShaders();

    ComPtr<ID3D11InputLayout> m_pVertexLayout;
    ComPtr<ID3D11VertexShader> m_pVertexShader;
    ComPtr<ID3D11PixelShader> m_pPixelShader;

    ComPtr<ID3D11Buffer> m_pVertexBuffer;
};

GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : D3DApp(hInstance, windowName, initWidth, initHeight)
{
}

bool GameApp::Init()
{
    if (!D3DApp::Init())
        return false;

    if (!InitShaders())
    {
        return false;
    }

    if (!InitResources())
    {
        return false;
    }

    return true;
}


void GameApp::OnResize()
{
    D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
}

void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);
    static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };  // RGBA = (0,0,255,255)
    m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_pd3dImmediateContext->Draw(3, 0);
    HR(m_pSwapChain->Present(0, 0));
    
}

bool GameApp::InitResources()
{

    VertexPosColor vertices[]={
        {DirectX::XMFLOAT3(0.0f,0.5f,0.5f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
        {DirectX::XMFLOAT3(0.5f,-0.5f,0.5f),DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
        {DirectX::XMFLOAT3(-0.5f,-0.5f,0.5f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    
    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = vertices;
    
    HR(m_pd3dDevice->CreateBuffer(&bd,&initData,m_pVertexBuffer.GetAddressOf()));

    UINT stride = sizeof(VertexPosColor);
    UINT offset = 0;

    m_pd3dImmediateContext->IASetVertexBuffers(0,1,m_pVertexBuffer.GetAddressOf(),&stride,&offset);
    m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

    m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), 0, 0);
    m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), 0, 0);


    m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
     // 设置调试对象名
    D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
    D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
    D3D11SetDebugObjectName(m_pVertexShader.Get(), "Trangle_VS");
    D3D11SetDebugObjectName(m_pPixelShader.Get(), "Trangle_PS");

    

    return true;
}

bool GameApp::InitShaders()
{
    ComPtr<ID3DBlob> blob = nullptr;

    HR(CreateShaderFromFile(L"Shaders/Triangle_VS.cso", L"Shaders/Triangle.hlsl", "VS", "vs_5_0", blob.GetAddressOf()));
    HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));


    m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout,ARRAYSIZE(VertexPosColor::inputLayout),
        blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf());

    HR(CreateShaderFromFile(L"Shaders/Triangle_PS.cso", L"Shaders/Triangle.hlsl", "PS", "ps_5_0", blob.GetAddressOf()));
    HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

    return true;
}

#include "target_path.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
    
    SetCurrentDirectoryA(TARGET_OUTPUT_PATH);
    char buf[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buf);
    
    // 取消未使用参数的编译警告
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);

    // 允许在Debug版本进行运行时内存分配和泄漏检测
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    
    GameApp theApp(hInstance, L"D3D11 Framework", 800, 600);

    if (!theApp.Init())
        return 0;

    return theApp.Run();
}
