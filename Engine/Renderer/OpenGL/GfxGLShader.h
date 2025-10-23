#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <memory>
#include <glad/glad.h>
#include "Renderer/Gfx.h"

namespace SoulEngine::Gfx
{
    class GLShaderModule final : public IShaderModule
    {
    public:
        GLShaderModule(GLenum stage, const char* source, const char* name);
        ~GLShaderModule() override;

        ShaderStage GetStage() const override { return stage_; }
        GLuint GetGLName() const { return shader_; }

    private:
        GLuint shader_ = 0;
        ShaderStage stage_{};
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
