#include "Renderer/Gfx.h"
#include <glad/gl.h>
#include <cassert>
namespace SoulEngine::Gfx::GL
{

    static GLenum ToTarget(BufferKind k)
    {
        switch (k)
        {
        case BufferKind::Vertex:
            return GL_ARRAY_BUFFER;
        case BufferKind::Index:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BufferKind::Constant:
            return GL_UNIFORM_BUFFER;
        // case BufferKind::Storage:  return GL_SHADER_STORAGE_BUFFER;
        // case BufferKind::Indirect: return GL_DRAW_INDIRECT_BUFFER;
        default:
            return GL_ARRAY_BUFFER;
        }
    }

    static GLenum ToUsage(BufferUsage u)
    {
        switch (u)
        {
        case BufferUsage::Immutable:
            return GL_STATIC_DRAW;
        case BufferUsage::Dynamic:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::Staging:
            return GL_STREAM_DRAW; // doc default
        case BufferUsage::Default:
        default:
            return GL_STATIC_DRAW;
        }
    }

    struct AttribInfo
    {
        GLenum type;
        GLint comps;
        GLboolean norm;
    };
    static AttribInfo ToAttribInfo(DataFormat fmt)
    {
        switch (fmt)
        {
        case DataFormat::R32_Float:
            return {GL_FLOAT, 1, GL_FALSE};
        case DataFormat::R32G32_Float:
            return {GL_FLOAT, 2, GL_FALSE};
        case DataFormat::R32G32B32_Float:
            return {GL_FLOAT, 3, GL_FALSE};
        case DataFormat::R32G32B32A32_Float:
            return {GL_FLOAT, 4, GL_FALSE};
        case DataFormat::R8G8B8A8_UNorm:
            return {GL_UNSIGNED_BYTE, 4, GL_TRUE};
        default:
            return {GL_FLOAT, 4, GL_FALSE};
        }
    }

    static GLenum ToIndexType(IndexFormat f)
    {
        return (f == IndexFormat::UInt16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    }

    static GLbitfield ToMapFlags(MapMode mode)
    {
        switch (mode)
        {
        case MapMode::Read:
            return GL_MAP_READ_BIT;
        case MapMode::Write:
            return GL_MAP_WRITE_BIT;
        case MapMode::WriteDiscard:
            return GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        case MapMode::WriteNoOverwrite:
            return GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
        }
        return GL_MAP_WRITE_BIT;
    }

    class GLBuffer final : public IBuffer
    {
    public:
        explicit GLBuffer(const BufferDesc &d) : desc(d) {}
        ~GLBuffer() override { Destroy(); }

        const BufferDesc &GetDesc() const override { return desc; }

        void Update(const SubresourceData &src) override
        {
            assert(id != 0);
            const GLenum target = ToTarget(desc.kind);
            glBindBuffer(target, id);
            if (src.offset == 0 && src.size == desc.size)
            {
                glBufferData(target, static_cast<GLsizeiptr>(src.size), src.data, ToUsage(desc.usage));
            }
            else
            {
                glBufferSubData(target, static_cast<GLintptr>(src.offset), static_cast<GLsizeiptr>(src.size), src.data);
            }
        }

        void *Map(MapMode mode) override
        {
            assert(id != 0);
            const GLenum target = ToTarget(desc.kind);
            glBindBuffer(target, id);
            return glMapBufferRange(target, 0, static_cast<GLsizeiptr>(desc.size), ToMapFlags(mode));
        }

        void Unmap() override
        {
            assert(id != 0);
            const GLenum target = ToTarget(desc.kind);
            glBindBuffer(target, id);
            glUnmapBuffer(target);
        }

        void Create(const SubresourceData *initial)
        {
            if (id)
                return;
            const GLenum target = ToTarget(desc.kind);
            glGenBuffers(1, &id);
            glBindBuffer(target, id);
            const GLsizeiptr sz = static_cast<GLsizeiptr>(desc.size);
            if (initial && initial->data && initial->size)
            {
                if (initial->offset == 0 && initial->size == desc.size)
                {
                    glBufferData(target, sz, initial->data, ToUsage(desc.usage));
                }
                else
                {
                    glBufferData(target, sz, nullptr, ToUsage(desc.usage));
                    glBufferSubData(target, static_cast<GLintptr>(initial->offset), static_cast<GLsizeiptr>(initial->size), initial->data);
                }
            }
            else
            {
                glBufferData(target, sz, nullptr, ToUsage(desc.usage));
            }
        }

        void Destroy()
        {
            if (id)
            {
                glDeleteBuffers(1, &id);
                id = 0;
            }
        }

        GLuint id = 0;
        BufferDesc desc{};
    };

    // ---- GLVertexInputLayout ----
    class GLVertexInputLayout final : public IVertexInputLayout
    {
    public:
        ~GLVertexInputLayout() override
        {
            if (vao)
                glDeleteVertexArrays(1, &vao);
        }
        GLuint vao = 0;
        std::vector<VertexAttribute> attributes;
    };

    // ---- GLDevice ----
    class GLDevice final : public IDevice
    {
    public:
        std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc &d, const SubresourceData *initial) override
        {
            auto buf = std::make_shared<GLBuffer>(d);
            buf->Create(initial);
            return buf;
        }

        std::shared_ptr<IVertexInputLayout> CreateVertexInputLayout(const VertexAttribute *attrs, uint32_t count) override
        {
            auto layout = std::make_shared<GLVertexInputLayout>();
            layout->attributes.assign(attrs, attrs + count);
            glGenVertexArrays(1, &layout->vao);
            return layout;
        }

        // std::size_t MinConstantBufferAlignment() const override
        // {
        //     GLint align = 0;
        //     glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);
        //     return static_cast<std::size_t>(align > 0 ? align : 256);
        // }
    };
    // ---- GLContext ----
    class GLContext final : public IContext
    {
    public:
        void SetVertexBuffers(uint32_t startSlot, IBuffer *const *buffers, const uint32_t *strides, const uint32_t *offsets, uint32_t count) override
        {
            // Save strides/offsets per binding slot (simplified: contiguous slots only).
            for (uint32_t i = 0; i < count; ++i)
            {
                const uint32_t slot = startSlot + i;
                bindingStrides[slot] = strides ? strides[i] : 0;
                bindingOffsets[slot] = offsets ? offsets[i] : 0;
                auto *buf = dynamic_cast<GLBuffer *>(buffers[i]);
                if (!buf)
                    continue;
                glBindBuffer(GL_ARRAY_BUFFER, buf->id);
                // Attribute pointers are set in SetVertexInputLayout using saved strides/offsets per attribute.
            }
        }

        void SetIndexBuffer(IBuffer *buffer, IndexFormat fmt) override
        {
            auto *buf = dynamic_cast<GLBuffer *>(buffer);
            if (!buf)
                return;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->id);
            currentIndexType = ToIndexType(fmt);
        }

        void SetConstantBuffer(uint32_t /*stage*/, uint32_t slot, IBuffer *buffer) override
        {
            auto *buf = dynamic_cast<GLBuffer *>(buffer);
            if (!buf)
                return;
            glBindBufferBase(GL_UNIFORM_BUFFER, slot, buf->id);
        }

        void SetVertexInputLayout(IVertexInputLayout *layout) override
        {
            auto *glLayout = dynamic_cast<GLVertexInputLayout *>(layout);
            if (!glLayout)
                return;
            glBindVertexArray(glLayout->vao);

            // Define vertex attribs relative to currently bound GL_ARRAY_BUFFER(s).
            for (const auto &a : glLayout->attributes)
            {
                const auto info = ToAttribInfo(a.format);
                const GLuint loc = a.location;
                glEnableVertexAttribArray(loc);
                const GLsizei stride = static_cast<GLsizei>(bindingStrides[a.bindingSlot]);
                const void *ptr = reinterpret_cast<const void *>(static_cast<uintptr_t>(a.offset + bindingOffsets[a.bindingSlot]));
                glVertexAttribPointer(loc, info.comps, info.type, info.norm, stride, ptr);
                // Note: a.stepRate > 1 implies instancing; use divisor 1 as a simple rule.
                if (a.stepRate > 1)
                    glVertexAttribDivisor(loc, 1);
            }
        }

        void Draw(uint32_t vertexCount, uint32_t startVertex) override
        {
            glDrawArrays(GL_TRIANGLES, static_cast<GLint>(startVertex), static_cast<GLsizei>(vertexCount));
        }

        void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex) override
        {
            const void *indexOffset = reinterpret_cast<const void *>(static_cast<uintptr_t>(startIndex) * (currentIndexType == GL_UNSIGNED_SHORT ? 2u : 4u));
            glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(indexCount), currentIndexType, indexOffset, baseVertex);
        }

    private:
        GLenum currentIndexType = GL_UNSIGNED_INT;
        uint32_t bindingStrides[16]{};
        uint32_t bindingOffsets[16]{};
    };

    // Gfx 实现将在这里添加
} // namespace SoulEngine::Gfx