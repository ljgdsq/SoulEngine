#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/OpenGL/GfxGLProgram.h"
#include "Renderer/OpenGL/GfxGLShader.h"
#include "Log/Logger.h"

namespace SoulEngine::Gfx
{
    GLProgram::GLProgram(const std::shared_ptr<GLShaderModule>& vs,
                         const std::shared_ptr<GLShaderModule>& fs,
                         const char* name)
    {
        program_ = glCreateProgram();
        glAttachShader(program_, vs->GetGLName());
        glAttachShader(program_, fs->GetGLName());
        glLinkProgram(program_);
        GLint ok = GL_FALSE;
        glGetProgramiv(program_, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            char log[2048]{};
            glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
            Logger::Error("Program link failed ({}): {}", name ? name : "", log);
        }
        Reflect();
    }

    GLProgram::~GLProgram()
    {
        if (program_)
            glDeleteProgram(program_);
    }

    void GLProgram::Reflect()
    {
        reflection_.uniforms.clear();
        reflection_.samplers.clear();

        GLint uniformCount = 0;
        glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &uniformCount);
        GLint maxNameLen = 0;
        glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);
        std::string name; name.resize(maxNameLen);

        for (GLint i = 0; i < uniformCount; ++i)
        {
            GLsizei nameLen = 0; GLint size = 0; GLenum type = 0;
            glGetActiveUniform(program_, i, maxNameLen, &nameLen, &size, &type, name.data());
            name.resize(nameLen);
            GLint loc = glGetUniformLocation(program_, name.c_str());

            UniformInfo u{}; u.name = name; u.location = loc; u.arraySize = size;
            switch (type)
            {
            case GL_FLOAT: u.type = UniformType::Float; break;
            case GL_FLOAT_VEC2: u.type = UniformType::Vec2; break;
            case GL_FLOAT_VEC3: u.type = UniformType::Vec3; break;
            case GL_FLOAT_VEC4: u.type = UniformType::Vec4; break;
            case GL_FLOAT_MAT4: u.type = UniformType::Mat4; break;
            case GL_INT: u.type = UniformType::Int; break;
            case GL_SAMPLER_2D:
            case GL_SAMPLER_CUBE:
            case GL_SAMPLER_2D_ARRAY:
            {
                SamplerInfo s{}; s.name = name; s.location = loc;
                reflection_.samplers.push_back(std::move(s));
                u.type = UniformType::Int; // samplers are ints, but list them separately
                break;
            }
            default: u.type = UniformType::Unknown; break;
            }
            reflection_.uniforms.push_back(std::move(u));
        }
    }

    int GLProgram::GetUniformLocation(const char* name) const
    {
        if (!name) return -1;
        auto it = uniformCache_.find(name);
        if (it != uniformCache_.end()) return it->second;
        GLint loc = glGetUniformLocation(program_, name);
        uniformCache_[name] = loc;
        return loc;
    }

    void GLProgram::SetTexture(const char* name, int slot)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform1i(loc, slot);
    }

    void GLProgram::SetFloat(const char* name, float v)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform1f(loc, v);
    }

    void GLProgram::SetInt(const char* name, int v)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform1i(loc, v);
    }

    void GLProgram::SetVec2(const char* name, const float* v2)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform2fv(loc, 1, v2);
    }

    void GLProgram::SetVec3(const char* name, const float* v3)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform3fv(loc, 1, v3);
    }

    void GLProgram::SetVec4(const char* name, const float* v4)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniform4fv(loc, 1, v4);
    }

    void GLProgram::SetMat4(const char* name, const float* m16, bool transpose)
    {
        const int loc = GetUniformLocation(name);
        if (loc >= 0) glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, m16);
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
