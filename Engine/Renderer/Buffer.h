#pragma once
#include <vector>

namespace SoulEngine
{
    enum class BufferType
    {
        VAO,
        VBO,
        EBO,
        FBO
    };

    enum class BufferUsage
    {
        StaticDraw,
        DynamicDraw,
        StreamDraw
    };

    class Buffer
    {
    public:
        Buffer(BufferType type);
        virtual ~Buffer() = default;
        virtual void BindBuffer() =0;
        virtual void UnbindBuffer() =0;
        virtual void Destroy() =0;
        virtual void UpdateData(const void* data, size_t size, BufferUsage usage = BufferUsage::StaticDraw) =0;

    protected:
        virtual void GenBuffer() =0;

    protected:
        bool valid;
        unsigned int id;
        BufferType type;
    };

    struct BufferLayout
    {
        int index;
        int nComp;
        int type;
        bool normalize;
        int stride;
        int offset;
    };

    class VertexArray : public Buffer
    {
    protected:
        std::vector<BufferLayout> layouts;

    public:
        VertexArray();
        ~VertexArray() override = default;
        void AddLayout(BufferLayout layout);
        void EnableAttributes(int startIndex = 0);
    };

    class FrameBuffer : public Buffer
    {
    protected:
        unsigned int colorAttachment;
        unsigned int rbo;
        unsigned int width;
        unsigned int height;

    public:
        FrameBuffer(unsigned int width, unsigned int height);
        virtual ~FrameBuffer() override;
        virtual void UpdateData(const void* data, size_t size, BufferUsage usage = BufferUsage::StaticDraw) override;
        virtual void Destroy() override;

        int GetWidth() const;
        int GetHeight() const;
        unsigned int GetColorBuffer() const;

    protected:
        void GenFrameBuffer();
    };
} // namespace SoulEngine
