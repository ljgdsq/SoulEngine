#pragma once

#if defined(SOULENGINE_ENABLE_OPENGL)

#include <glad/glad.h>
#include <cstdint>
#include "Renderer/Gfx.h"

namespace SoulEngine::Gfx
{
    inline GLenum ToGLBufferTarget(BufferKind kind)
    {
        switch (kind)
        {
        case BufferKind::Vertex:   return GL_ARRAY_BUFFER;
        case BufferKind::Index:    return GL_ELEMENT_ARRAY_BUFFER;
        case BufferKind::Constant: return GL_UNIFORM_BUFFER;
        case BufferKind::Storage:  return GL_SHADER_STORAGE_BUFFER;
        case BufferKind::Indirect: return GL_DRAW_INDIRECT_BUFFER;
        case BufferKind::Staging:  return GL_COPY_READ_BUFFER; // simple default
        default: return GL_ARRAY_BUFFER;
        }
    }

    inline GLenum ToGLUsage(BufferUsage usage)
    {
        switch (usage)
        {
        case BufferUsage::Default:   return GL_STATIC_DRAW;
        case BufferUsage::Immutable: return GL_STATIC_DRAW;
        case BufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
        case BufferUsage::Staging:   return GL_STREAM_READ;
        default: return GL_STATIC_DRAW;
        }
    }

    inline GLenum ToGLIndexType(IndexFormat fmt)
    {
        switch (fmt)
        {
        case IndexFormat::UInt16: return GL_UNSIGNED_SHORT;
        case IndexFormat::UInt32: return GL_UNSIGNED_INT;
        default: return GL_UNSIGNED_INT;
        }
    }

    struct GLFormatInfo { GLenum type; GLint size; GLboolean normalized; };

    inline GLFormatInfo ToGLVertexFormat(DataFormat fmt)
    {
        switch (fmt)
        {
        case DataFormat::R32_Float:          return { GL_FLOAT, 1, GL_FALSE };
        case DataFormat::R32G32_Float:       return { GL_FLOAT, 2, GL_FALSE };
        case DataFormat::R32G32B32_Float:    return { GL_FLOAT, 3, GL_FALSE };
        case DataFormat::R32G32B32A32_Float: return { GL_FLOAT, 4, GL_FALSE };
        case DataFormat::R8G8B8A8_UNorm:     return { GL_UNSIGNED_BYTE, 4, GL_TRUE };
        default: return { GL_FLOAT, 4, GL_FALSE };
        }
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
