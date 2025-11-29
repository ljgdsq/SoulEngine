
#include "d3dApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace DirectX;
struct VertexPosColor
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct ConstantBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
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
    ComPtr<ID3D11Buffer> m_pIndexBuffer;
    ComPtr<ID3D11Buffer> m_pConstantBuffer;

    ConstantBuffer m_ConstantBuffer;
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

    static float phi = 0.0f, theta = 0.0f;
    phi += 0.3f * dt, theta += 0.37f * dt;
    m_ConstantBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
    // 更新常量缓冲区，让立方体转起来
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memcpy_s(mappedData.pData, sizeof(m_ConstantBuffer), &m_ConstantBuffer, sizeof(m_ConstantBuffer));
    m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
}

void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);
    static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };    // RGBA = (0,0,0,255)
    m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // m_pd3dImmediateContext->Draw(3, 0);
    m_pd3dImmediateContext->DrawIndexed(36,0,0);
    HR(m_pSwapChain->Present(0, 0));
    
}

bool GameApp::InitResources()
{

    // ******************
    // 设置立方体顶点
    //    5________ 6
    //    /|      /|
    //   /_|_____/ |
    //  1|4|_ _ 2|_|7
    //   | /     | /
    //   |/______|/
    //  0       3
    VertexPosColor vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
    };
    // 索引数组
    DWORD indices[] = {
        // 正面
        0, 1, 2,
        2, 3, 0,
        // 左面
        4, 5, 1,
        1, 0, 4,
        // 顶面
        1, 5, 6,
        6, 2, 1,
        // 背面
        7, 6, 5,
        5, 4, 7,
        // 右面
        3, 2, 6,
        6, 7, 3,
        // 底面
        4, 0, 3,
        3, 7, 4
    };
    
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    
    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = vertices;
    HR(m_pd3dDevice->CreateBuffer(&bd,&initData,m_pVertexBuffer.GetAddressOf()));


    D3D11_BUFFER_DESC ibd{};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData{};
    iinitData.pSysMem = indices;
    HR(m_pd3dDevice->CreateBuffer(&ibd, &iinitData, m_pIndexBuffer.GetAddressOf()));
    m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    D3D11_BUFFER_DESC cbd{};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // Create the constant buffer , no initial data
    HR(m_pd3dDevice->CreateBuffer(&cbd,nullptr,m_pConstantBuffer.GetAddressOf()));
    

    m_ConstantBuffer.world = XMMatrixIdentity();
    m_ConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
        XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    ));

    
    m_ConstantBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));
    

    UINT stride = sizeof(VertexPosColor);
    UINT offset = 0;

    m_pd3dImmediateContext->IASetVertexBuffers(0,1,m_pVertexBuffer.GetAddressOf(),&stride,&offset);
    m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

    m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);

    m_pd3dImmediateContext->VSSetConstantBuffers(0,1,m_pConstantBuffer.GetAddressOf());

    m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
    
     // 设置调试对象名
    D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
    D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
    D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
    D3D11SetDebugObjectName(m_pConstantBuffer.Get(), "ConstantBuffer");
    D3D11SetDebugObjectName(m_pVertexShader.Get(), "Cube_VS");
    D3D11SetDebugObjectName(m_pPixelShader.Get(), "Cube_PS");

    // // 在 InitResources 末尾添加，用于测试
    // D3D11_RASTERIZER_DESC rs{};
    // rs.FillMode = D3D11_FILL_SOLID;
    // rs.CullMode = D3D11_CULL_NONE; // 关闭剔除，测试
    // rs.DepthClipEnable = TRUE;
    // ComPtr<ID3D11RasterizerState> rsState;
    // HR(m_pd3dDevice->CreateRasterizerState(&rs, rsState.GetAddressOf()));
    // m_pd3dImmediateContext->RSSetState(rsState.Get());

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
