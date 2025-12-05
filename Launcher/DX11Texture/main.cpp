
#include <DWrite.h>

#include "d3dApp.h"
#include "d3dUtil.h"
#include "DDSTextureLoader11.h"
#include "DXTrace.h"
#include "LightHelper.h"
#include "Geometry.h"
#include "WICTextureLoader11.h"
using namespace DirectX;
struct VSConstantBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
    DirectX::XMMATRIX worldInvTranspose;
};

struct PSConstantBuffer
{
    DirectionalLight dirLight[10];
    PointLight pointLight[10];
    SpotLight spotLight[10];
    Material material;
    int numDirLight;
    int numPointLight;
    int numSpotLight;
    float pad;		// 打包保证16字节对齐
    DirectX::XMFLOAT4 eyePos;
};
namespace
{
    bool animateCube = true;
    bool customColor = false;
    float tx = 0.0f, ty = 0.0f;
    float scale = 1.0f;
    float phi = 0.0f, theta = 0.0f;
    float fov = XM_PIDIV2;
}
enum class ShowMode { WoodCrate, FireAnim };
class GameApp : public D3DApp
{
public:
    GameApp(HINSTANCE hInstance, const std::wstring &windowName, int initWidth, int initHeight);
    ~GameApp() override = default;

    bool Init() override;
    void OnResize() override;
    void UpdateScene(float dt) override;
    void DrawScene() override;

    bool InitResources();
    bool InitShaders();

    template<class VertexType>
    bool ResetMesh(const Geometry::MeshData<VertexType> &meshData);
    ComPtr<ID3D11InputLayout> m_pVertexLayout2D;  // 顶点输入布局
    ComPtr<ID3D11InputLayout> m_pVertexLayout3D;  // 顶点输入布局
    ComPtr<ID3D11Buffer> m_pVertexBuffer;       // 顶点缓冲区
    ComPtr<ID3D11Buffer> m_pIndexBuffer;        // 索引缓冲区
    ComPtr<ID3D11Buffer> m_pConstantBuffers[2]; // 常量缓冲区
    UINT m_IndexCount;                          // 绘制物体的索引数组大小
    int m_CurrFrame;	
    ShowMode m_CurrMode = ShowMode::WoodCrate;		
    ComPtr<ID3D11ShaderResourceView> m_pWoodCrate;
    std::vector<ComPtr<ID3D11ShaderResourceView>> m_pFireAnims; // 火焰纹理集
    ComPtr<ID3D11SamplerState> m_pSamplerState;

    ComPtr<ID3D11VertexShader> m_pVertexShader2D; // 顶点着色器
    ComPtr<ID3D11VertexShader> m_pVertexShader3D; // 顶点着色器
    ComPtr<ID3D11PixelShader> m_pPixelShader2D;   // 像素着色器
    ComPtr<ID3D11PixelShader> m_pPixelShader3D;   // 像素着色器
    VSConstantBuffer m_VSConstantBuffer;        // 用于修改用于VS的GPU常量缓冲区的变量
    PSConstantBuffer m_PSConstantBuffer;        // 用于修改用于PS的GPU常量缓冲区的变量

    DirectionalLight m_DirLight; // 默认环境光
    PointLight m_PointLight;     // 默认点光
    SpotLight m_SpotLight;       // 默认汇聚光

    ComPtr<ID3D11RasterizerState> m_pRSWireframe; // 光栅化状态: 线框模式
    bool m_IsWireframeMode;
};

GameApp::GameApp(HINSTANCE hInstance, const std::wstring &windowName, int initWidth, int initHeight)
    : D3DApp(hInstance, windowName, initWidth, initHeight),
      m_IndexCount(),
      m_VSConstantBuffer(),
      m_PSConstantBuffer(),
      m_DirLight(),
      m_PointLight(),
      m_SpotLight(),
      m_IsWireframeMode(false)
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
    if (ImGui::Begin("Texture Mapping"))
    {
        static int curr_mode_item = static_cast<int>(m_CurrMode);
        const char* mode_strs[] = {
            "Box",
            "Fire Anim"
        };
        if (ImGui::Combo("Mode", &curr_mode_item, mode_strs, ARRAYSIZE(mode_strs)))
        {
            if (curr_mode_item == 0)
            {
                // 播放木箱动画
                m_CurrMode = ShowMode::WoodCrate;
                m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout3D.Get());
                auto meshData = Geometry::CreateBox();
                ResetMesh(meshData);
                m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
                m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
                m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCrate.GetAddressOf());
            }
            else
            {
                m_CurrMode = ShowMode::FireAnim;
                m_CurrFrame = 0;
                m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout2D.Get());
                auto meshData = Geometry::Create2DShow();
                ResetMesh(meshData);
                m_pd3dImmediateContext->VSSetShader(m_pVertexShader2D.Get(), nullptr, 0);
                m_pd3dImmediateContext->PSSetShader(m_pPixelShader2D.Get(), nullptr, 0);
                m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireAnims[0].GetAddressOf());
            }
        }
    }
    ImGui::End();

    if (m_CurrMode == ShowMode::WoodCrate)
    {
        static float phi = 0.0f, theta = 0.0f;
        phi += 0.0001f, theta += 0.00015f;
        XMMATRIX W = XMMatrixRotationX(phi) * XMMatrixRotationY(theta);
        m_VSConstantBuffer.world = XMMatrixTranspose(W);
        m_VSConstantBuffer.worldInvTranspose = XMMatrixTranspose(InverseTranspose(W));

        // 更新常量缓冲区，让立方体转起来
        D3D11_MAPPED_SUBRESOURCE mappedData;
        HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
        memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
        m_pd3dImmediateContext->Unmap(m_pConstantBuffers[0].Get(), 0);
    }
    else if (m_CurrMode == ShowMode::FireAnim)
    {
        // 用于限制在1秒60帧
        static float totDeltaTime = 0;

        totDeltaTime += dt;
        if (totDeltaTime > 1.0f / 60)
        {
            totDeltaTime -= 1.0f / 60;
            m_CurrFrame = (m_CurrFrame + 1) % 120;
            m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pFireAnims[m_CurrFrame].GetAddressOf());
        }		
    }
}
void GameApp::DrawScene()
{
    assert(m_pd3dImmediateContext);
    assert(m_pSwapChain);
    static float black[4] = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA = (0,0,0,255)
    m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // 绘制几何模型
    m_pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
    // 可以在这之前调用ImGui的UI部分
    // Direct3D 绘制部分

    ImGui::Render();
    // 下面这句话会触发ImGui在Direct3D的绘制
    // 因此需要在此之前将后备缓冲区绑定到渲染管线上
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    HR(m_pSwapChain->Present(0, 0));
}
bool GameApp::InitShaders()
{
    ComPtr<ID3DBlob> blob = nullptr;

    HR(CreateShaderFromFile(L"Shaders/Basic_2D_VS.cso", L"Shaders/Basic_2D_VS.hlsl", "VS", "vs_5_0", blob.GetAddressOf()));
    HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader2D.GetAddressOf()));

    m_pd3dDevice->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
                                    blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout2D.GetAddressOf());

    HR(CreateShaderFromFile(L"Shaders/Basic_2D_PS.cso", L"Shaders/Basic_2D_PS.hlsl", "PS", "ps_5_0", blob.GetAddressOf()));
    HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader2D.GetAddressOf()));


    // 创建顶点着色器(3D)
    HR(CreateShaderFromFile(L"Shaders\\Basic_3D_VS.cso", L"Shaders\\Basic_3D_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
    HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader3D.GetAddressOf()));
    // 创建顶点布局(3D)
    HR(m_pd3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
        blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout3D.GetAddressOf()));

    // 创建像素着色器(3D)
    HR(CreateShaderFromFile(L"Shaders\\Basic_3D_PS.cso", L"Shaders\\Basic_3D_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
    HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader3D.GetAddressOf()));

    
    return true;
}



bool GameApp::InitResources()
{

    // ******************
    // 初始化网格模型
    //
    auto meshData = Geometry::CreateBox<VertexPosNormalColor>();
    ResetMesh(meshData);


    // ******************
    // 设置常量缓冲区描述
    //
    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(VSConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    // 新建用于VS和PS的常量缓冲区
    HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf()));
    cbd.ByteWidth = sizeof(PSConstantBuffer);
    HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf()));

    
    // ******************
    // 初始化纹理和采样器状态

    
    // 初始化木箱纹理

    HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"..\\..\\Resources\\Texture\\WoodCrate.dds", nullptr, m_pWoodCrate.GetAddressOf()));
    WCHAR strFile[128];
    m_pFireAnims.resize(120);
    for (int i = 1; i <= 120; i++)
    {
        wsprintf(strFile, L"..\\..\\Resources\\Texture\\FireAnim\\Fire%03d.bmp", i);
        CreateWICTextureFromFile(m_pd3dDevice.Get(), strFile, nullptr,m_pFireAnims[i-1].GetAddressOf());
    }

    // 初始化采样器状态
    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));
    

    // ******************
    // 初始化常量缓冲区的值
    //

    // 初始化用于VS的常量缓冲区的值
    m_VSConstantBuffer.world = XMMatrixIdentity();			
    m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
        XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    ));
    m_VSConstantBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));
    m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();
    
    // 初始化用于PS的常量缓冲区的值
    // 这里只使用一盏点光来演示
    m_PSConstantBuffer.pointLight[0].position = XMFLOAT3(0.0f, 0.0f, -10.0f);
    m_PSConstantBuffer.pointLight[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    m_PSConstantBuffer.pointLight[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    m_PSConstantBuffer.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    m_PSConstantBuffer.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
    m_PSConstantBuffer.pointLight[0].range = 25.0f;
    m_PSConstantBuffer.numDirLight = 0;
    m_PSConstantBuffer.numPointLight = 1;
    m_PSConstantBuffer.numSpotLight = 0;
    // 初始化材质
    m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_PSConstantBuffer.material.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 5.0f);
    // 注意不要忘记设置此处的观察位置，否则高亮部分会有问题
    m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);

    // 更新PS常量缓冲区资源
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
    m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);

    // ******************
    // 给渲染管线各个阶段绑定好所需资源
    // 设置图元类型，设定输入布局
    m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout3D.Get());
    // 默认绑定3D着色器
    m_pd3dImmediateContext->VSSetShader(m_pVertexShader3D.Get(), nullptr, 0);
    // VS常量缓冲区对应HLSL寄存于b0的常量缓冲区
    m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
    // PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
    m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
    // 像素着色阶段设置好采样器
    m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
    m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCrate.GetAddressOf());
    m_pd3dImmediateContext->PSSetShader(m_pPixelShader3D.Get(), nullptr, 0);
    
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
template<class VertexType>
bool GameApp::ResetMesh(const Geometry::MeshData<VertexType>& meshData)
{
    // 释放旧资源
    m_pVertexBuffer.Reset();
    m_pIndexBuffer.Reset();



    // 设置顶点缓冲区描述
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexType);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    // 新建顶点缓冲区
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = meshData.vertexVec.data();
    HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.ReleaseAndGetAddressOf()));

    // 输入装配阶段的顶点缓冲区设置
    UINT stride = sizeof(VertexType);			// 跨越字节数
    UINT offset = 0;							// 起始偏移量

    m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);



    // 设置索引缓冲区描述
    m_IndexCount = (UINT)meshData.indexVec.size();
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * m_IndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    // 新建索引缓冲区
    InitData.pSysMem = meshData.indexVec.data();
    HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.ReleaseAndGetAddressOf()));
    // 输入装配阶段的索引缓冲区设置
    m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



    // 设置调试对象名
    D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
    D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

    return true;
}
