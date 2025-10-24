#include <SoulEngine.h>
#include "Core/ApplicationHelper.h"
#include "Core/Timer.h"
#include "EngineFileIO.h"
#include "Input.h"

using namespace SoulEngine;
using namespace SoulEngine::Gfx;

class MyApplication : public SoulEngine::ApplicationBase
{
public:
    MyApplication() : ApplicationBase("SoulEngine OpenGL Demo")
    {
    }

    std::shared_ptr<IProgram> program;
    std::shared_ptr<IVertexInputLayout> layout;
    std::shared_ptr<IBuffer> vertexBuffer1;
    std::shared_ptr<IBuffer> vertexBuffer2;
    std::shared_ptr<IBuffer> indexBuffer;

    bool OnInitialize() override
    {
        Logger::Log("MyApplication initialized");
        auto device = GetDevice();
        auto context = GetContext();
        auto vsCode = EngineFileIO::LoadText("Shaders/base2.vs.glsl");
        auto fsCode = EngineFileIO::LoadText("Shaders/base2.fs.glsl");

        auto vs = device->CreateShaderModule({Gfx::ShaderStage::Vertex, vsCode.c_str(), "base"});
        auto fs = device->CreateShaderModule({Gfx::ShaderStage::Fragment, fsCode.c_str(), "base"});
        program = device->CreateProgram(vs, fs, "base");

        struct Vertex
        {
            float pos[3];
            float color[3];
        };
        struct Pos
        {
            float x, y, z;
        };

        struct Color
        {
            float r, g, b;
        };

        std::vector<Pos> positions = {
            {0.5f, 0.5f, 0.0f},
            {0.5f, -0.5f, 0.0f},
            {-0.5f, -0.5f, 0.0f},
            {-0.5f, 0.5f, 0.0f}
        };

        std::vector<Color> colors = {
            {1.f, 0.f, 0.f},
            {0.f, 1.f, 0.f},
            {0.f, 0.f, 1.f},
            {0.f, 0.f, 1.f},
        };

        std::vector<uint16_t> indices = {
            0, 1, 2,
            2, 3, 0
        };


        Gfx::BufferDesc indexBufferDesc{};
        indexBufferDesc.size = indices.size() * sizeof(uint16_t);
        indexBufferDesc.kind = Gfx::BufferKind::Index;
        indexBufferDesc.usage = Gfx::BufferUsage::Immutable;
        indexBufferDesc.bindFlags = Gfx::BindFlags::IndexBuffer;
        auto indexData = Gfx::SubresourceData{indices.data(), indexBufferDesc.size, 0};
        indexBuffer = device->CreateBuffer(indexBufferDesc, &indexData);
        context->SetIndexBuffer(indexBuffer.get(), Gfx::IndexFormat::UInt16);


        Gfx::BufferDesc bufferDesc1{};
        bufferDesc1.size = positions.size() * sizeof(Pos);
        bufferDesc1.kind = Gfx::BufferKind::Vertex;
        bufferDesc1.usage = Gfx::BufferUsage::Immutable;
        bufferDesc1.bindFlags = Gfx::BindFlags::VertexBuffer;
        auto data = Gfx::SubresourceData{positions.data(), bufferDesc1.size, 0};
        vertexBuffer1 = device->CreateBuffer(bufferDesc1, &data);


        Gfx::BufferDesc bufferDesc2{};
        bufferDesc2.size = colors.size() * sizeof(Color);
        bufferDesc2.kind = Gfx::BufferKind::Vertex;
        bufferDesc2.usage = Gfx::BufferUsage::Immutable;
        bufferDesc2.bindFlags = Gfx::BindFlags::VertexBuffer;
        auto data2 = Gfx::SubresourceData{colors.data(), bufferDesc2.size, 0};
        vertexBuffer2 = device->CreateBuffer(bufferDesc2, &data2);

        Gfx::VertexAttribute attrs[] = {
            {0, Gfx::DataFormat::R32G32B32_Float, 0, 0,},
            {1, Gfx::DataFormat::R32G32B32_Float, 0, 1},
        };

        uint32_t strides[] = {sizeof(float) * 3, sizeof(float) * 3};
        uint32_t offsets[] = {0, 0};
        auto vertexBufferPtr1 = vertexBuffer1.get();
        auto vertexBufferPtr2 = vertexBuffer2.get();
        IBuffer* vertexBuffers[] = {vertexBufferPtr1, vertexBufferPtr2};
        context->SetVertexBuffers(0, vertexBuffers, strides, offsets, 2);

        layout = device->CreateVertexInputLayout(attrs, 2);
        context->SetVertexInputLayout(layout.get());
        return true;
    }

    void Update(float deltaTime) override
    {

        Logger::Log("time: {:.2f} deltaTime: {:.4f}", Timer::GetInstance().GetElapsedTime(), deltaTime);
        
        if (Input::GetKeyDown(KeyCode::Escape))
        {
            m_window->SetShouldClose(true);
        }

        if (Input::GetKeyDown(KeyCode::F1))
        {
            GetContext()->SetPolygonMode(PolygonMode::Fill);
        }
        else if (Input::GetKeyDown(KeyCode::F2))
        {
            GetContext()->SetPolygonMode(PolygonMode::Line);
        }
        else if (Input::GetKeyDown(KeyCode::F3))
        {
            GetContext()->SetPolygonMode(PolygonMode::Point);
        }

        static bool dynamicColor = true;
        if (dynamicColor)
        {
            float time = Timer::GetInstance().GetElapsedTime();
            float g = (sin(time) + 1.0f) / 2.0f;
            float c[] = {1.0f, 0.0f, g, 1.0f};
            program->SetVec4("u_Color", c);
        }

        
        if (Input::GetKeyDown(KeyCode::F5))
        {
            dynamicColor = !dynamicColor;
    
        }
        else if (Input::GetKeyDown(KeyCode::F7))
        {
            float c[] = {1.0f, 1.0f, 1.0f, 1.0f};
            program->SetVec4("u_Color", c);
        }
    }

    void Render() override
    {
        auto context = GetContext();
        context->BindProgram(program.get());
        context->SetVertexInputLayout(layout.get());
        context->DrawIndexed(6, 0, 0);
    }

    void Shutdown() override
    {
        SoulEngine::Logger::Log("MyApplication shutdown");
    }

    bool ShouldClose() const override
    {
        return GetWindow()->ShouldClose();
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(MyApplication)
