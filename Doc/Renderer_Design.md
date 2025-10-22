# SoulEngine Renderer Design (Cross-API, Cross-Platform)

This document proposes a clean, backend-agnostic renderer abstraction focused first on buffer and vertex input encapsulation, then on device/context responsibilities. It avoids leaking OpenGL-only concepts (like VAO) into public headers and enables adding Direct3D/Vulkan backends later without changing engine-side code.

## Goals

- Backend-agnostic public API: no GL/DX/Vulkan headers or types in public engine headers.
- Correct abstraction boundaries: separate data resources (buffers) from input state (vertex layout/VAO-like) and pipeline state.
- Simple and efficient updates: unified Update/Map semantics across APIs.
- Scalable: easy to extend to constant/storage/indirect buffers, framebuffers, and more.
- Predictable ownership and lifecycle.

## Design Principles

- Public API uses engine enums/structs; backends translate to native equivalents.
- No Bind/Unbind on resources; binding is the job of the command/context.
- OpenGL VAO stays inside the GL backend; engine uses a backend-agnostic VertexInputLayout.
- Resource creation via Device; usage via Context; no global state.

## Core Abstractions

- Device: resource factory and capability query.
- Context: command encoder for binding/draw/update operations.
- Buffer: contiguous GPU resource for vertices, indices, constants, storage, indirect, etc.
- VertexInputLayout: describes vertex attributes/layout; backend may internally use VAO (GL) or input layouts (DX) or vertex input state (VK).

### Key Enums and Structs (engine-owned)

- BufferKind: Vertex, Index, Constant, Storage, Indirect, Staging
- BufferUsage: Default, Immutable, Dynamic, Staging
- CpuAccessFlags (bitmask): None, Read, Write
- BindFlags (bitmask): VertexBuffer, IndexBuffer, ConstantBuffer, ShaderResource, UnorderedAccess, Indirect
- IndexFormat: UInt16, UInt32
- DataFormat (vertex attribute): R32_Float, R32G32_Float, R32G32B32_Float, R32G32B32A32_Float, R8G8B8A8_UNorm, …
- BufferDesc:
  - size: size_t
  - kind: BufferKind
  - usage: BufferUsage
  - bindFlags: BindFlags
  - cpuAccess: CpuAccessFlags
  - stride: optional (bytes per element, for structured buffers)
  - indexFormat: optional for index buffers
  - name: optional debug name
- SubresourceData:
  - data: const void*
  - size: size_t
  - offset: size_t (default 0)
- MapMode: Read, Write, WriteDiscard, WriteNoOverwrite

### Public Interfaces (sketch)

- class IBuffer
  - const BufferDesc& GetDesc() const
  - void Update(const SubresourceData& src)                // offset + size supported
  - void* Map(MapMode mode)
  - void Unmap()

- struct VertexAttribute
  - location (or semantic), format (DataFormat), offset (bytes), bindingSlot, stepRate (1 per-vertex, 0 or instance rate)

- class IVertexInputLayout
  - created from a span/array of VertexAttribute

- class IDevice
  - std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc&, const SubresourceData* initial = nullptr)
  - std::shared_ptr<IVertexInputLayout> CreateVertexInputLayout(const VertexAttribute* attrs, uint32_t count)
  - capability queries (e.g., MinConstantBufferAlignment())

- class IContext
  - void SetVertexBuffers(uint32_t startSlot, span<IBuffer*>, span<uint32_t> strides, span<uint32_t> offsets)
  - void SetIndexBuffer(IBuffer*, IndexFormat)
  - void SetConstantBuffer(uint32_t stage, uint32_t slot, IBuffer*)
  - void SetVertexInputLayout(IVertexInputLayout*)
  - void Draw(uint32_t vertexCount, uint32_t startVertex)
  - void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex)

Note: The API sketches omit namespaces and some details for brevity.

## Why VAO Should Not Be a Buffer

- VAO is OpenGL-specific and represents input state, not a data resource.
- D3D uses input layouts + vertex buffers; Vulkan uses vertex input state.
- Therefore, use VertexInputLayout as a public concept; OpenGL backend may implement it with an internal VAO.

## Backend Mappings

### OpenGL

- BufferKind → GL targets:
  - Vertex → GL_ARRAY_BUFFER
  - Index → GL_ELEMENT_ARRAY_BUFFER
  - Constant → GL_UNIFORM_BUFFER
  - Storage → GL_SHADER_STORAGE_BUFFER
  - Indirect → GL_DRAW_INDIRECT_BUFFER
- BufferUsage/CpuAccess → GL usage and mapping flags:
  - Immutable → glBufferData (no Map)
  - Dynamic → GL_DYNAMIC_DRAW + glMapBufferRange (WRITE, often WriteDiscard)
  - Staging → GL_COPY_READ/WRITE_BUFFER + Map as needed
- VertexInputLayout → internal VAO (optional caching), or set vertex attrib pointers on bind.
- DataFormat → pair of (GL type + component count/normalized flag).
- Constant buffer alignment → query GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT and expose via Device.

GL include tips:
- Define GLFW_INCLUDE_NONE (via compile definitions) to prevent GLFW from including system GL headers.
- Include order in source files: <glad/gl.h> before any GLFW window headers if used.

### Direct3D (11/12) – Outline

- Usage/CpuAccess map to D3D usage and CPU access flags.
- VertexInputLayout maps to D3D_INPUT_ELEMENT_DESC.
- Constant buffer alignment: 16 bytes; validate in Device.
- Dynamic updates: Map with D3D11_MAP_WRITE_DISCARD/NO_OVERWRITE or persistent upload heaps in D3D12.

### Vulkan – Outline

- Buffer usages map to VkBufferUsageFlags.
- Memory allocation strategy: simple allocator initially (e.g., one allocation per buffer) or integrate VMA later.
- VertexInputLayout → VkVertexInput* structs.
- Updates: staging buffers + vkCmdCopyBuffer, or host-visible memory with proper sync.

## Example Usage (Engine Side)

```cpp
// Create vertex buffer
BufferDesc vb{};
vb.size = sizeof(Vertex) * vertexCount;
vb.kind = BufferKind::Vertex;
vb.usage = BufferUsage::Immutable;
vb.bindFlags = BindFlags::VertexBuffer;
auto vertexBuffer = device->CreateBuffer(vb, SubresourceData{vertices, vb.size, 0});

// Create index buffer
BufferDesc ib{};
ib.size = sizeof(uint32_t) * indexCount;
ib.kind = BufferKind::Index;
ib.usage = BufferUsage::Immutable;
ib.bindFlags = BindFlags::IndexBuffer;
ib.indexFormat = IndexFormat::UInt32;
auto indexBuffer = device->CreateBuffer(ib, SubresourceData{indices, ib.size, 0});

// Vertex layout
VertexAttribute attrs[] = {
  { .location=0, .format=DataFormat::R32G32B32_Float, .offset=0,  .bindingSlot=0, .stepRate=1 },
  { .location=1, .format=DataFormat::R32G32_Float,    .offset=12, .bindingSlot=0, .stepRate=1 },
};
auto layout = device->CreateVertexInputLayout(attrs, 2);

// Bind and draw
context->SetVertexBuffers(0, {vertexBuffer.get()}, {sizeof(Vertex)}, {0});
context->SetIndexBuffer(indexBuffer.get(), IndexFormat::UInt32);
context->SetVertexInputLayout(layout.get());
context->DrawIndexed(indexCount, 0, 0);
```

## Ownership, Lifetime, and Threading

- Device creates resources; return shared_ptr by default (simple ownership). Context holds non-owning raw pointers during a command list scope.
- If you adopt multi-threaded recording later, introduce command lists and fences; for now, single immediate context is fine.
- Provide Destroy() only on backend internals; public resources release via RAII.

## Error Handling

- Failures in CreateBuffer/CreateVertexInputLayout return nullptr; log with error channel.
- Map/Update with invalid ranges assert in debug builds; return gracefully (no-op) in release with error log.

## Migration Plan (from current code)

1. Remove BufferType::VAO/FBO from public API; treat Framebuffer separately (future IFramebuffer/RenderPass docs).
2. Replace current Buffer/VertexArray headers with IBuffer/IVertexInputLayout abstractions (new Gfx headers under Engine/Renderer/Gfx/...).
3. Implement OpenGL backend:
   - GLBuffer (implements IBuffer), GLVertexInputLayout (implements IVertexInputLayout, internally uses VAO), GLDevice, GLContext.
   - Keep existing GLFW window/context creation; just wire Device/Context to current renderer.
4. Update engine code to create buffers via Device and bind via Context. Remove Buffer::Bind/Unbind from public interface.
5. Add CMake option for backend selection; keep OpenGL as default. Ensure GLFW_INCLUDE_NONE is set for targets that include GLFW.

## Next Steps

- If this proposal looks good, I’ll:
  - Scaffold the Gfx interfaces and enums (headers only, no backend code yet).
  - Implement the minimal OpenGL backend (Buffer + VertexInputLayout + bind/draw path) and adapt one sample to use it.
  - Provide a small README on how to toggle backends via CMake.

---
Questions or tweaks before I start coding? Happy to adjust naming, ownership, or the initial scope.