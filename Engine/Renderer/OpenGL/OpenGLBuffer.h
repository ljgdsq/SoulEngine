#include "Renderer/Buffer.h"

namespace SoulEngine
{
    class OpenGLBuffer : public Buffer
    {
    public:
        explicit OpenGLBuffer(BufferType type);
    public:
        void BindBuffer() override;
        void UnbindBuffer() override;
        void Destroy() override;
        void UpdateData(const void* data, size_t size, BufferUsage usage) override;
    protected:
        void GenBuffer() override;
    };


    class OpenGLVertexArray : public VertexArray
    {
    public:
        
    };
    
}
