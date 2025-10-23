#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/OpenGL/GfxGLDevice.h"
#include "Renderer/OpenGL/GfxGLCommon.h"
#include "Renderer/OpenGL/GfxGLVertexInputLayout.h"
#include "Renderer/OpenGL/GfxGLBuffer.h"
#include "Renderer/OpenGL/GfxGLShader.h"
#include "Renderer/OpenGL/GfxGLProgram.h"
#include <glad/glad.h>

namespace SoulEngine::Gfx
{
    std::shared_ptr<IBuffer> GfxGLDevice::CreateBuffer(const BufferDesc& desc, const SubresourceData* initial)
    {
        auto buf = std::make_shared<GLBuffer>(desc);
        buf->Initialize(initial);
        return buf;
    }

    std::shared_ptr<IVertexInputLayout> GfxGLDevice::CreateVertexInputLayout(const VertexAttribute* attrs, uint32_t count)
    {
        return std::make_shared<GLVertexInputLayout>(attrs, count);
    }

    std::shared_ptr<IShaderModule> GfxGLDevice::CreateShaderModule(const ShaderDesc& desc)
    {
        GLenum stage = GL_VERTEX_SHADER;
        switch (desc.stage)
        {
        case ShaderStage::Vertex:   stage = GL_VERTEX_SHADER; break;
        case ShaderStage::Fragment: stage = GL_FRAGMENT_SHADER; break;
        case ShaderStage::Geometry: stage = GL_GEOMETRY_SHADER; break;
        case ShaderStage::Compute:  stage = GL_COMPUTE_SHADER; break;
        }
        auto mod = std::make_shared<GLShaderModule>(stage, desc.source, desc.name);
        return mod;
    }

    std::shared_ptr<IProgram> GfxGLDevice::CreateProgram(const std::shared_ptr<IShaderModule>& vs,
                                                         const std::shared_ptr<IShaderModule>& fs,
                                                         const char* name)
    {
        auto glvs = std::static_pointer_cast<GLShaderModule>(vs);
        auto glfs = std::static_pointer_cast<GLShaderModule>(fs);
        return std::make_shared<GLProgram>(glvs, glfs, name);
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
