#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <glad/glad.h>
#include "Renderer/Gfx.h"
#include "Renderer/OpenGL/GfxGLCommon.h"

namespace SoulEngine::Gfx
{
    class GLBuffer final : public IBuffer
    {
    public:
        explicit GLBuffer(const BufferDesc& desc);
        ~GLBuffer() override;

        const BufferDesc& GetDesc() const override { return desc_; }

        void Initialize(const SubresourceData* initial);
        void Update(const SubresourceData& src) override;
        void* Map(MapMode mode) override;
        void Unmap() override;

        GLuint GetGLName() const { return buffer_; }
        GLenum GetTarget() const { return target_; }

    private:
        BufferDesc desc_{};
        GLuint buffer_ = 0;
        GLenum target_ = GL_ARRAY_BUFFER;
        void* mapped_ = nullptr;
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
