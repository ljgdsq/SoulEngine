#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/OpenGL/GfxGLBuffer.h"

namespace SoulEngine::Gfx
{
    GLBuffer::GLBuffer(const BufferDesc& desc)
        : desc_(desc)
    {
        glGenBuffers(1, &buffer_);
        target_ = ToGLBufferTarget(desc_.kind);
    }

    GLBuffer::~GLBuffer()
    {
        if (buffer_)
            glDeleteBuffers(1, &buffer_);
    }

    void GLBuffer::Initialize(const SubresourceData* initial)
    {
        glBindBuffer(target_, buffer_);
        if (initial && initial->data && initial->size)
        {
            glBufferData(target_, static_cast<GLsizeiptr>(initial->size), initial->data, ToGLUsage(desc_.usage));
        }
        else
        {
            glBufferData(target_, static_cast<GLsizeiptr>(desc_.size), nullptr, ToGLUsage(desc_.usage));
        }
    }

    void GLBuffer::Update(const SubresourceData& src)
    {
        glBindBuffer(target_, buffer_);
        glBufferSubData(target_, static_cast<GLintptr>(src.offset), static_cast<GLsizeiptr>(src.size), src.data);
    }

    void* GLBuffer::Map(MapMode mode)
    {
        glBindBuffer(target_, buffer_);
        GLbitfield access = 0;
        switch (mode)
        {
        case MapMode::Read: access = GL_MAP_READ_BIT; break;
        case MapMode::Write: access = GL_MAP_WRITE_BIT; break;
        case MapMode::WriteDiscard: access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; break;
        case MapMode::WriteNoOverwrite: access = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT; break;
        }
        mapped_ = glMapBufferRange(target_, 0, static_cast<GLsizeiptr>(desc_.size), access);
        return mapped_;
    }

    void GLBuffer::Unmap()
    {
        if (mapped_)
        {
            glBindBuffer(target_, buffer_);
            glUnmapBuffer(target_);
            mapped_ = nullptr;
        }
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
