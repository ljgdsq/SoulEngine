#pragma once
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace SoulEngine::Gfx
{
    enum class BufferKind : uint8_t { Vertex, Index, Constant, Storage, Indirect, Staging };
    enum class BufferUsage : uint8_t { Default, Immutable, Dynamic, Staging };

    enum class CpuAccessFlags : uint8_t { None = 0, Read = 1 << 0, Write = 1 << 1 };
    inline CpuAccessFlags operator|(CpuAccessFlags a, CpuAccessFlags b) {
        return static_cast<CpuAccessFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    enum class BindFlags : uint32_t {
        None = 0,
        VertexBuffer     = 1u << 0,
        IndexBuffer      = 1u << 1,
        ConstantBuffer   = 1u << 2,
        ShaderResource   = 1u << 3,
        UnorderedAccess  = 1u << 4,
        Indirect         = 1u << 5,
    };

    inline BindFlags operator|(BindFlags a, BindFlags b) {
        return static_cast<BindFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    enum class IndexFormat : uint8_t { UInt16, UInt32 };

    enum class DataFormat : uint16_t {
        Unknown = 0,
        R32_Float,
        R32G32_Float,
        R32G32B32_Float,
        R32G32B32A32_Float,
        R8G8B8A8_UNorm,
    };

    enum class ShaderStage : uint8_t { Vertex, Fragment, Geometry, Compute };

    struct BufferDesc
    {
        std::size_t  size=0;
        BufferKind  kind=BufferKind::Vertex;
        BufferUsage usage=BufferUsage::Default;
        BindFlags   bindFlags=BindFlags::None;
        CpuAccessFlags cpuAccess=CpuAccessFlags::None;
        std::size_t stride=0;                 // optional, for structured buffers
        IndexFormat indexFormat=IndexFormat::UInt32; // only for Index buffers
        const char* name=nullptr;             // debug name
    };
    
    struct SubresourceData
    {
        const void* data=nullptr;
        std::size_t size=0;
        std::size_t offset=0;
    };

    enum class MapMode : uint8_t { Read, Write, WriteDiscard, WriteNoOverwrite };

    struct VertexAttribute
    {
        uint32_t location;
        DataFormat format;
        uint32_t offset;
        uint32_t bindingSlot;
        uint32_t stepRate;
    };

    struct ShaderDesc {
        ShaderStage stage;
        const char* source;      // UTF-8 source code string
        const char* entryPoint;  // ignored for GL; required for some backends
        const char* name;        // debug name
    };

    enum class UniformType : uint8_t { Float, Int, Vec2, Vec3, Vec4, Mat4, Unknown };

    struct UniformInfo {
        std::string name;
        int location = -1;
        UniformType type = UniformType::Unknown;
        int arraySize = 1;
    };

    struct SamplerInfo {
        std::string name;
        int location = -1; // usually a uniform location
    };

    struct ProgramReflection {
        std::vector<UniformInfo> uniforms;
        std::vector<SamplerInfo> samplers;
    };




    class IBuffer {
    public:
        virtual ~IBuffer() = default;
        virtual const BufferDesc& GetDesc() const = 0;
        virtual void Update(const SubresourceData& src) = 0;
        virtual void* Map(MapMode mode) = 0;
        virtual void Unmap() = 0;
    };

    class IVertexInputLayout {
    public:
        virtual ~IVertexInputLayout() = default;
    };

    class IShaderModule {
    public:
        virtual ~IShaderModule() = default;
        virtual ShaderStage GetStage() const = 0;
    };

    class IProgram {
    public:
        virtual ~IProgram() = default;
        virtual const ProgramReflection& GetReflection() const = 0;
        // Convenience, name-based uniform setters (backend should bind current program as needed or assume already bound)
        virtual void SetTexture(const char* name, int slot) = 0;
        virtual void SetFloat(const char* name, float v) = 0;
        virtual void SetInt(const char* name, int v) = 0;
        virtual void SetVec2(const char* name, const float* v2) = 0;
        virtual void SetVec3(const char* name, const float* v3) = 0;
        virtual void SetVec4(const char* name, const float* v4) = 0;
        virtual void SetMat4(const char* name, const float* m16, bool transpose=false) = 0;
    };

    class IDevice {
    public:
        virtual ~IDevice() = default;
        virtual std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc& desc, const SubresourceData* initial) = 0;
        virtual std::shared_ptr<IVertexInputLayout> CreateVertexInputLayout(const VertexAttribute* attrs, uint32_t count) = 0;
        virtual std::shared_ptr<IShaderModule> CreateShaderModule(const ShaderDesc& desc) = 0;
        virtual std::shared_ptr<IProgram> CreateProgram(const std::shared_ptr<IShaderModule>& vs,
                                                       const std::shared_ptr<IShaderModule>& fs,
                                                       const char* name = nullptr) = 0;
    };

    class IContext {
    public:
        virtual ~IContext() = default;
        virtual void SetVertexBuffers(uint32_t startSlot, IBuffer* const* buffers, const uint32_t* strides, const uint32_t* offsets, uint32_t count) = 0;
        virtual void SetIndexBuffer(IBuffer* buffer, IndexFormat fmt) = 0;
        virtual void SetConstantBuffer(uint32_t stage, uint32_t slot, IBuffer* buffer) = 0;
        virtual void SetVertexInputLayout(IVertexInputLayout* layout) = 0;
        virtual void BindProgram(IProgram* program) = 0;
        virtual void Draw(uint32_t vertexCount, uint32_t startVertex) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex) = 0;

    };

}