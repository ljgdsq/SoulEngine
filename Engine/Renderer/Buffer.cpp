#include "Buffer.h"

namespace SoulEngine
{
    Buffer::Buffer(BufferType type)
    {
        this->type = type;
        this->id = 0;
        valid = false;
    }

    VertexArray::VertexArray():Buffer(BufferType::VAO)
    {
    }

    void VertexArray::AddLayout(BufferLayout layout)
    {
        if (layout.index >= layouts.size())
        {
            layouts.resize(layout.index + 10);
        }
        layouts[layout.index] = layout;
    }
}
