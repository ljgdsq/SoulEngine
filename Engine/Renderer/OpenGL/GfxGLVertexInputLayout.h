#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <vector>
#include <cstdint>
#include "Renderer/Gfx.h"
#include "Renderer/OpenGL/GfxGLCommon.h"

namespace SoulEngine::Gfx
{
    class GLVertexInputLayout final : public IVertexInputLayout
    {
    public:
        GLVertexInputLayout(const VertexAttribute* attrs, uint32_t count)
            : attributes_(attrs, attrs + count)
        {
            glGenVertexArrays(1, &vao_);
        }

        ~GLVertexInputLayout() override
        {
            if (vao_)
                glDeleteVertexArrays(1, &vao_);
        }

        GLuint GetVAO() const { return vao_; }
        const std::vector<VertexAttribute>& GetAttributes() const { return attributes_; }
        uint32_t GetBindingCount() const { return bindingCount_; }

    private:
        GLuint vao_ = 0;
        std::vector<VertexAttribute> attributes_;
        uint32_t bindingCount_ = 0; // optional: could compute max binding + 1
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
