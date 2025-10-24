#include <SoulEngine.h>

#include "EngineFileIO.h"
#include "Renderer/RenderSystem.h"
#include "Input.h"
using namespace SoulEngine;
using namespace SoulEngine::Gfx;
class MyApplication : public SoulEngine::Application
{
public:
    MyApplication() : Application("SoulEngine OpenGL Demo") {}

    std::shared_ptr<IProgram> program;
    std::shared_ptr<IVertexInputLayout> layout;
    std::shared_ptr<IBuffer> vertexBuffer; // 保存 Buffer 的生命周期
    bool Initialize(Engine* engine) override
    {
        this->engine = engine;
        Logger::Log("MyApplication initialized");
        auto device = engine->GetSystem<RenderSystem>()->GetDevice();
        auto context = engine->GetSystem<RenderSystem>()->GetContext();
        auto vsCode=EngineFileIO::LoadText("Shaders/base.vs.glsl");
        auto fsCode=EngineFileIO::LoadText("Shaders/base.fs.glsl");

        auto vs=device->CreateShaderModule({Gfx::ShaderStage::Vertex,vsCode.c_str(),"base"});
        auto fs=device->CreateShaderModule({Gfx::ShaderStage::Fragment,fsCode.c_str(),"base"});
        program = device->CreateProgram(vs,fs,"base");

        struct Vertex { float pos[3]; float color[3]; };
        // Vertex data
        std::vector<Vertex> vertices = {
            {{-0.6f,-0.5f,0.f}, {1.f,0.f,0.f}},
            {{ 0.6f,-0.5f,0.f}, {0.f,1.f,0.f}},
            {{ 0.0f, 0.6f,0.f}, {0.f,0.f,1.f}},
        };
        
        
        Gfx::BufferDesc bufferDesc{};
        bufferDesc.size = vertices.size() * sizeof(Vertex);
        bufferDesc.kind = Gfx::BufferKind::Vertex;
        bufferDesc.usage = Gfx::BufferUsage::Immutable;
        bufferDesc.bindFlags = Gfx::BindFlags::VertexBuffer;
        auto data=Gfx::SubresourceData{vertices.data(), bufferDesc.size, 0};
        vertexBuffer = device->CreateBuffer(bufferDesc,&data); // 保存 shared_ptr

        Gfx::VertexAttribute attrs[]={
            {0,Gfx::DataFormat::R32G32B32_Float,offsetof(Vertex,pos),0,},
            {1,Gfx::DataFormat::R32G32B32_Float,offsetof(Vertex,color),0},
        };
        layout=device->CreateVertexInputLayout(attrs,2);

        context->SetVertexInputLayout(layout.get());       
        uint32_t strides[] = {sizeof(float) * 6}; // pos[3] + color[3] = 6 floats
        uint32_t offsets[] = {0};
        auto vertexBufferPtr = vertexBuffer.get();
        context->SetVertexBuffers(0, &vertexBufferPtr, strides, offsets, 1);
        
        return true;
    }

    void Update(float deltaTime) override
    {
        if (Input::GetInstance().GetKeyDown(KeyCode::Escape))
        {
            m_window->SetShouldClose(true);
        }
            
    }

    void Render() override
    {
        auto context = engine->GetSystem<RenderSystem>()->GetContext();
        context->BindProgram(program.get());
        context->SetVertexInputLayout(layout.get());
        context->Draw(3,0);
    }

    void Shutdown() override
    {
        SoulEngine::Logger::Log("MyApplication shutdown");
    }

    bool ShouldClose() const override
    {
        return m_window->ShouldClose();
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(MyApplication)