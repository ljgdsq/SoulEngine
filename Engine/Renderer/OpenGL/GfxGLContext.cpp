#if defined(SOULENGINE_ENABLE_OPENGL)

#include "Renderer/OpenGL/GfxGLContext.h"
#include "Renderer/OpenGL/GfxGLVertexInputLayout.h"
#include "Renderer/OpenGL/GfxGLCommon.h"
#include "Renderer/OpenGL/GfxGLBuffer.h"
#include "Renderer/OpenGL/GfxGLProgram.h"
#include <glad/glad.h>

namespace SoulEngine::Gfx
{
    void GfxGLContext::SetVertexBuffers(uint32_t startSlot, IBuffer* const* buffers, const uint32_t* strides, const uint32_t* offsets, uint32_t count)
    {
        if (startSlot != 0)
            return; // simple implementation supports startSlot=0 only for now

        vbSlots_.resize(count);
        strides_.assign(strides, strides + count);
        offsets_.assign(offsets, offsets + count);

        for (uint32_t i = 0; i < count; ++i)
        {
            vbSlots_[i] = static_cast<GLBuffer*>(buffers[i]);
        }

        ApplyVertexArrayBindings();
    }

    void GfxGLContext::SetIndexBuffer(IBuffer* buffer, IndexFormat fmt)
    {
        indexBuffer_ = static_cast<GLBuffer*>(buffer);
        indexFormat_ = fmt;
        ApplyVertexArrayBindings();
    }

    void GfxGLContext::SetConstantBuffer(uint32_t /*stage*/, uint32_t slot, IBuffer* buffer)
    {
        // Bind to UBO binding point (slot)
        auto* glbuf = static_cast<GLBuffer*>(buffer);
        if (glbuf)
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, slot, glbuf->GetGLName());
        }
    }

    void GfxGLContext::SetVertexInputLayout(IVertexInputLayout* layout)
    {
        currentLayout_ = static_cast<GLVertexInputLayout*>(layout);
        ApplyVertexArrayBindings();
    }

    void GfxGLContext::BindProgram(IProgram* program)
    {
        auto* glp = static_cast<GLProgram*>(program);
        if (glp)
        {
            glUseProgram(glp->GetGLName());
        }
    }

    void GfxGLContext::Draw(uint32_t vertexCount, uint32_t startVertex)
    {
        if (!currentLayout_) return;
        glBindVertexArray(currentLayout_->GetVAO());
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(startVertex), static_cast<GLsizei>(vertexCount));
    }

    void GfxGLContext::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex)
    {
        if (!currentLayout_ || !indexBuffer_) return;
        glBindVertexArray(currentLayout_->GetVAO());
        const GLenum type = ToGLIndexType(indexFormat_);
        const void* indices = reinterpret_cast<const void*>(static_cast<uintptr_t>(startIndex * (type == GL_UNSIGNED_SHORT ? 2u : 4u)));
        glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(indexCount), type, indices, baseVertex);
    }
   // todo: 优化项 : 缓存当前绑定状态，避免重复绑定
    void GfxGLContext::ApplyVertexArrayBindings()
    {
        if (!currentLayout_)
            return;

        glBindVertexArray(currentLayout_->GetVAO());

        // Configure attributes for current bound vertex buffers
        const auto& attrs = currentLayout_->GetAttributes();
        for (const auto& a : attrs)
        {
            const uint32_t slot = a.bindingSlot;
            if (slot >= vbSlots_.size() || vbSlots_[slot] == nullptr)
                continue;

            const GLuint vbo = vbSlots_[slot]->GetGLName();
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            const auto fmt = ToGLVertexFormat(a.format);
            const GLsizei stride = static_cast<GLsizei>(strides_.size() > slot ? strides_[slot] : 0);
            const GLsizei attribStride = stride;
            const GLvoid* pointer = reinterpret_cast<const GLvoid*>(static_cast<uintptr_t>((offsets_.size() > slot ? offsets_[slot] : 0) + a.offset));

            glEnableVertexAttribArray(a.location);
            glVertexAttribPointer(a.location, fmt.size, fmt.type, fmt.normalized, attribStride, pointer);
            // Optional: instance rate handling via glVertexAttribDivisor if stepRate > 0
            if (a.stepRate > 0)
                glVertexAttribDivisor(a.location, a.stepRate);
        }

        // Bind index buffer to VAO state if available
        if (indexBuffer_)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_->GetGLName());
        }

        // Unbind array buffer to avoid accidental state leakage
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GfxGLContext::SetPolygonMode(PolygonMode mode)
    {
        GLenum glMode;
        switch (mode)
        {
            case PolygonMode::Fill:  glMode = GL_FILL; break;
            case PolygonMode::Line:  glMode = GL_LINE; break;
            case PolygonMode::Point: glMode = GL_POINT; break;
            default: return;
        }
        glPolygonMode(GL_FRONT_AND_BACK, glMode);
    }

    void GfxGLContext::SetCullMode(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:
                glDisable(GL_CULL_FACE);
                break;
            case CullMode::Front:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                break;
            case CullMode::Back:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                break;
        }
    }

    void GfxGLContext::SetBlendMode(BlendMode mode)
    {
        switch (mode)
        {
            case BlendMode::None:
                glDisable(GL_BLEND);
                break;
            case BlendMode::Alpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendMode::Additive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
        }
    }

    void GfxGLContext::SetDepthTest(bool enable)
    {
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void GfxGLContext::SetScissorTest(bool enable, int x, int y, int width, int height)
    {
        if (enable)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(x, y, width, height);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }
}

#endif // SOULENGINE_ENABLE_OPENGL
