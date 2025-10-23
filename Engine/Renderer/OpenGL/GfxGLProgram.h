#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "Renderer/Gfx.h"

namespace SoulEngine::Gfx
{
    class GLShaderModule;

    class GLProgram final : public IProgram
    {
    public:
        GLProgram(const std::shared_ptr<GLShaderModule>& vs,
                  const std::shared_ptr<GLShaderModule>& fs,
                  const char* name);
        ~GLProgram() override;

        const ProgramReflection& GetReflection() const override { return reflection_; }

        GLuint GetGLName() const { return program_; }

    // Convenience setters (assumes program is bound by context)
    void SetTexture(const char* name, int slot) override;
    void SetFloat(const char* name, float v) override;
    void SetInt(const char* name, int v) override;
    void SetVec2(const char* name, const float* v2) override;
    void SetVec3(const char* name, const float* v3) override;
    void SetVec4(const char* name, const float* v4) override;
    void SetMat4(const char* name, const float* m16, bool transpose=false) override;

        int GetUniformLocation(const char* name) const;

    private:
        GLuint program_ = 0;
        ProgramReflection reflection_{};
        mutable std::unordered_map<std::string, int> uniformCache_;

        void Reflect();
    };
}

#endif // SOULENGINE_ENABLE_OPENGL
