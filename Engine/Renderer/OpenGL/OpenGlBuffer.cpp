#include "OpenGLBuffer.h"

#include "glad/glad.h"
namespace SoulEngine
{
    OpenGLBuffer::OpenGLBuffer(BufferType type):Buffer(type)
    {
    }

    void OpenGLBuffer::BindBuffer()
    {
        if (!valid)
            return;
        switch (type)
        {
        case BufferType::VAO:
            glBindVertexArray(id);
            break;
        case BufferType::VBO:
            glBindBuffer(GL_ARRAY_BUFFER, id);
            break;
        case BufferType::EBO:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
            break;
        case BufferType::FBO:
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            break;
        }
    }

    void OpenGLBuffer::UnbindBuffer()
    {
        if (!valid)
            return;
        switch (type)
        {
        case BufferType::VAO:
            glBindVertexArray(0);
            break;
        case BufferType::VBO:
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            break;
        case BufferType::EBO:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            break;
        case BufferType::FBO:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        }
    }

    void OpenGLBuffer::Destroy()
    {
        if (!valid)
            return;
        switch (type)
        {
        case BufferType::VAO:
            glDeleteVertexArrays(1, &id);
            break;
        case BufferType::VBO:
            glDeleteBuffers(1, &id);
            break;
        case BufferType::EBO:
            glDeleteBuffers(1, &id);
            break;
        case BufferType::FBO:
            glDeleteFramebuffers(1, &id);
            break;
        }
        valid = false;
    }

    void OpenGLBuffer::UpdateData(const void* data, size_t size, BufferUsage usage)
    {
        if (!valid)
            return;
        BindBuffer();
        switch (type)
        {
        case BufferType::VBO:
            glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<GLenum>(usage));
            break;
        case BufferType::EBO:
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, static_cast<GLenum>(usage));
            break;
        default:
            // VAO and FBO do not use UpdateData
            break;
        }
    }

    void OpenGLBuffer::GenBuffer()
    {
        if (valid)
            return;
        switch (type)
        {
        case BufferType::VAO:
            glGenVertexArrays(1, &id);
            break;
        case BufferType::VBO:
        case BufferType::EBO:
            glGenBuffers(1, &id);
            break;
        case BufferType::FBO:
            glGenFramebuffers(1, &id);
            break;
        }
        valid = true;
    }
}
