#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <memory>
#include "Renderer/Gfx.h"

namespace SoulEngine::Gfx
{
    class GLBuffer;
    class GLVertexInputLayout;

    class GfxGLDevice final : public IDevice
    {
    public:
        GfxGLDevice() = default;
        ~GfxGLDevice() override = default;

        std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc& desc, const SubresourceData* initial) override;
        std::shared_ptr<IVertexInputLayout> CreateVertexInputLayout(const VertexAttribute* attrs, uint32_t count) override;
        std::shared_ptr<IShaderModule> CreateShaderModule(const ShaderDesc& desc) override;
        std::shared_ptr<IProgram> CreateProgram(const std::shared_ptr<IShaderModule>& vs,
                                               const std::shared_ptr<IShaderModule>& fs,
                                               const char* name = nullptr) override;
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
