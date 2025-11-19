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
#include <memory>
#include "D3DApp.h"
#include "Logger.h"
#include "UploadBuffer.h"

using Microsoft::WRL::ComPtr;
#pragma warning(disable:4355)


struct  ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj=MathHelper::Identity4x4();
};

    

struct Vertex{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct Vertex2{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 Tex0;
    DirectX::XMFLOAT2 Tex1;

};

class BoxApp: public D3DApp
{
public:
    BoxApp(HINSTANCE hInstance);
    ~BoxApp();
    bool Initialize()override;
private:
    virtual void OnResize()override;
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;

    virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildPSO();
private:
    ComPtr<ID3D12RootSignature> mRootSignature=nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap=nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB=nullptr;
    std::unique_ptr<MeshGeometry> mBoxGeo=nullptr;

    ComPtr<ID3DBlob> mvsByteCode=nullptr;
    ComPtr<ID3DBlob> mpsByteCode=nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    ComPtr<ID3D12PipelineState> mPSO=nullptr;

    DirectX::XMFLOAT4X4 mWorld= MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mView= MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj= MathHelper::Identity4x4();

    float mTheta = 1.5f*DirectX::XM_PI;
    float mPhi = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
    
};

BoxApp::BoxApp(HINSTANCE hInstance): D3DApp(hInstance)
{
}

BoxApp::~BoxApp()= default;

bool BoxApp::Initialize()
{
    if (!D3DApp::Initialize())
    {
        return false;
    }

    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();


    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    FlushCommandQueue();
    return true;
}

void BoxApp::OnResize()
{
    D3DApp::OnResize();
    DirectX::XMMATRIX P=DirectX::XMMatrixPerspectiveFovLH(0.25f*DirectX::XM_PI, AspectRatio(),1.0f,1000.0f);
    DirectX::XMStoreFloat4x4(&mProj,P);
}

void BoxApp::Update(const GameTimer& gt)
{
    D3DApp::Update(gt);

    //根据球坐标（半径、俯仰角、方位角）计算摄像机（或观察点）在三维空间中的位置
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    //构建观察矩阵
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mView, view);
    
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorld);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
    DirectX::XMMATRIX worldViewProj = world * view * proj;

    //用于更新常量缓冲区的数据
    ObjectConstants objConstants;
    DirectX::XMStoreFloat4x4(&objConstants.WorldViewProj, DirectX::XMMatrixTranspose(worldViewProj));
    mObjectCB->CopyData(0, objConstants);
    
}

void BoxApp::Draw(const GameTimer &gt)
{
    ThrowIfFailed(mDirectCmdListAlloc->Reset());
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

    mCommandList->RSSetViewports(1,&mScreenViewport);
    mCommandList->RSSetScissorRects(1,&mScissorRect);

    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET));

    mCommandList->ClearRenderTargetView(CurrentBackBufferView(),DirectX::Colors::LightSteelBlue,0,nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(),D3D12_CLEAR_FLAG_DEPTH|D3D12_CLEAR_FLAG_STENCIL,1.0f,0,0,nullptr);

    mCommandList->OMSetRenderTargets(1,&CurrentBackBufferView(),TRUE, &DepthStencilView());

    ID3D12DescriptorHeap* descriptorHeaps[]={mCbvHeap.Get()};
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps),descriptorHeaps);

    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    mCommandList->IASetVertexBuffers(0,1,&mBoxGeo->VertexBufferView());
    mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mCommandList->SetGraphicsRootDescriptorTable(0,mCbvHeap->GetGPUDescriptorHandleForHeapStart());
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount,1,0,0,0);

    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(mCommandList->Close());

    ID3D12CommandList* cmdsLists[]={mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists),cmdsLists);

    ThrowIfFailed(mSwapChain->Present(0,0));
    mCurrBackBuffer = (mCurrBackBuffer +1)% SwapChainBufferCount;
    FlushCommandQueue();
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseDown(btnState, x, y);
    mLastMousePos.x=x;
    mLastMousePos.y=y;
    SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseUp(btnState, x, y);
    ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseMove(btnState, x, y);
    if (btnState & MK_LBUTTON )
    {
        float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

        mTheta += dx;
        mPhi += dy;

        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi -0.1f);
    }else if (btnState & MK_RBUTTON)
    {
        float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);
        mRadius += dx-dy;
        mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
         
    }

    mLastMousePos.x=x;
    mLastMousePos.y=y;

}

void BoxApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;

    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,IID_PPV_ARGS(&mCbvHeap)));
    
    
}

void BoxApp::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(),1,true);
    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress= mObjectCB->Resource()->GetGPUVirtualAddress();

    int boxCBufIndex=0;
    cbAddress += boxCBufIndex* objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation=cbAddress;
    cbvDesc.SizeInBytes=objCBByteSize;

    md3dDevice->CreateConstantBufferView(&cbvDesc,mCbvHeap->GetCPUDescriptorHandleForHeapStart());
    
    
}

void BoxApp::BuildRootSignature()
{
    CD3DX12_ROOT_PARAMETER slotRootPara[1];

    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,1,0);
    slotRootPara[0].InitAsDescriptorTable(1,&cbvTable);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1,slotRootPara,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    HRESULT hr=D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,serializedRootSig.GetAddressOf(),errorBlob.GetAddressOf());
    if (errorBlob!=nullptr)
    {
        SoulEngine::Logger::Error((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(md3dDevice->CreateRootSignature(0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));

}

void BoxApp::BuildShadersAndInputLayout()
{
    HRESULT hr=S_OK;
    mvsByteCode = d3dUtil::CompileShader(L"color.hlsl",nullptr,"VS","vs_5_0");
    mpsByteCode = d3dUtil::CompileShader(L"color.hlsl",nullptr,"PS","ps_5_0");

    mInputLayout = {
        {
            "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
        {
            "COLOR", 0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        }
    };
}

void BoxApp::BuildBoxGeometry()
{
    std::array<Vertex,8> vertices={
        Vertex({DirectX::XMFLOAT3(-1.0f,-1.0f,-1.0f),DirectX::XMFLOAT4(DirectX::Colors::White)}),
        Vertex({DirectX::XMFLOAT3(-1.0f,+1.0f,-1.0f),DirectX::XMFLOAT4(DirectX::Colors::Black)}),
        Vertex({DirectX::XMFLOAT3(+1.0f,+1.0f,-1.0f),DirectX::XMFLOAT4(DirectX::Colors::Red)}),
        Vertex({DirectX::XMFLOAT3(+1.0f,-1.0f,-1.0f),DirectX::XMFLOAT4(DirectX::Colors::Green)}),
        Vertex({DirectX::XMFLOAT3(-1.0f,-1.0f,+1.0f),DirectX::XMFLOAT4(DirectX::Colors::Blue)}),
        Vertex({DirectX::XMFLOAT3(-1.0f,+1.0f,+1.0f),DirectX::XMFLOAT4(DirectX::Colors::Yellow)}),
        Vertex({DirectX::XMFLOAT3(+1.0f,+1.0f,+1.0f),DirectX::XMFLOAT4(DirectX::Colors::Cyan)}),
        Vertex({DirectX::XMFLOAT3(+1.0f,-1.0f,+1.0f),DirectX::XMFLOAT4(DirectX::Colors::Magenta)}),
    };

    std::array<std::uint16_t,36> indices={
        0,1,2,
        0,2,3,
        
        4,6,5,
        4,7,6,
        
        4,5,1,
        4,1,0,
        
        3,2,6,
        3,6,7,

        1,5,6,
        1,6,2,
        4,0,3,
        4,3,7
    };

    const UINT vbByteSize= (UINT)vertices.size()*sizeof(Vertex);
    const UINT ibByteSize= (UINT)indices.size()*sizeof(std::uint16_t);

    mBoxGeo = std::make_unique<MeshGeometry>();
    mBoxGeo->Name="boxGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize,&mBoxGeo->VertexBufferCPU));
    CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(),vertices.data(),vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize,&mBoxGeo->IndexBufferCPU));
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(),indices.data(),ibByteSize);

    mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),mCommandList.Get(),vertices.data(),vbByteSize,mBoxGeo->VertexBufferUploader);

    mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),mCommandList.Get(),indices.data(),ibByteSize,mBoxGeo->IndexBufferUploader);

    mBoxGeo->VertexByteStride = sizeof(Vertex);
    mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->IndexBufferByteSize = ibByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;

    SubmeshGeometry submesh;
    submesh.IndexCount = indices.size();
    submesh.StartIndexLocation =0;
    submesh.BaseVertexLocation =0;

    mBoxGeo->DrawArgs["box"] = submesh;
    
}

void BoxApp::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = {mInputLayout.data(),(UINT)mInputLayout.size()};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = {
        reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
        mvsByteCode->GetBufferSize()
    };

    psoDesc.PS = {
        reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
        mpsByteCode->GetBufferSize()
    };

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = mDepthStencilFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4:1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality -1):0;
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&mPSO)));
    
    
}

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


    try
    {
        BoxApp app(hInstance);
        if (app.Initialize())
        {
            return app.Run();
        }
        
    }
    catch(const DxException& e)
    {
        MessageBoxW(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    }
    catch(const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Exception", MB_OK);
    }
    


     spdlog::info("DX12App finished frames");

    float exitDelay = 2.0f; // seconds
    spdlog::info("Exiting in {} seconds...", exitDelay);
    Sleep(static_cast<DWORD>(exitDelay * 1000));

    return 0;
}

