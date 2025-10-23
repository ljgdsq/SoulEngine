#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/Gfx.h"
#include <cstdint>
#include <vector>

namespace SoulEngine::Gfx
{
    class GLBuffer;
    class GLVertexInputLayout;

    class GfxGLContext final : public IContext
    {
    public:
        GfxGLContext() = default;
        ~GfxGLContext() override = default;

        void SetVertexBuffers(uint32_t startSlot, IBuffer* const* buffers, const uint32_t* strides, const uint32_t* offsets, uint32_t count) override;
        void SetIndexBuffer(IBuffer* buffer, IndexFormat fmt) override;
        void SetConstantBuffer(uint32_t stage, uint32_t slot, IBuffer* buffer) override;
        void SetVertexInputLayout(IVertexInputLayout* layout) override;
        void BindProgram(IProgram* program) override;
        void Draw(uint32_t vertexCount, uint32_t startVertex) override;
        void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex) override;

    private:
        GLVertexInputLayout* currentLayout_ = nullptr;
        std::vector<GLBuffer*> vbSlots_;
        std::vector<uint32_t> strides_;
        std::vector<uint32_t> offsets_;
        GLBuffer* indexBuffer_ = nullptr;
        IndexFormat indexFormat_ = IndexFormat::UInt32;

        void ApplyVertexArrayBindings();
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
