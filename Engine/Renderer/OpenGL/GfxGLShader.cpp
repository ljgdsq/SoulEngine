#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/OpenGL/GfxGLShader.h"
#include "Log/Logger.h"

namespace SoulEngine::Gfx
{
    static GLenum ToGLShaderStage(ShaderStage s)
    {
        switch (s)
        {
        case ShaderStage::Vertex:   return GL_VERTEX_SHADER;
        case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
        // case ShaderStage::Compute:  return GL_COMPUTE_SHADER;
        default: return GL_VERTEX_SHADER;
        }
    }

    GLShaderModule::GLShaderModule(GLenum stage, const char* source, const char* name)
    {
        shader_ = glCreateShader(stage);
        glShaderSource(shader_, 1, &source, nullptr);
        glCompileShader(shader_);
        GLint ok = GL_FALSE;
        glGetShaderiv(shader_, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[2048]{};
            glGetShaderInfoLog(shader_, sizeof(log), nullptr, log);
            Logger::Error("Shader compile failed ({}): {}", name ? name : "", log);
        }
    }

    GLShaderModule::~GLShaderModule()
    {
        if (shader_)
            glDeleteShader(shader_);
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
