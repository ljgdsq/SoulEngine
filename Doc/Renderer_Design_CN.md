# SoulEngine 渲染器设计（跨平台 / 跨图形 API）

本文档给出一套后端无关（OpenGL / Direct3D / Vulkan 可替换）的渲染抽象方案，优先解决 Buffer 与顶点输入布局的正确封装，并规划 Device/Context 的职责划分，避免将 OpenGL 专属概念（如 VAO）泄漏到公共接口。

## 设计目标

- 后端无关的公共 API：公共头文件不出现 GL/DX/VK 的头或类型。
- 正确抽象边界：将“数据资源（Buffer）”与“输入状态（顶点布局/VAO 类）”分离。
- 统一更新语义：跨 API 的 Update/Map 行为一致，便于调用方使用。
- 易扩展：逐步覆盖 Constant/Storage/Indirect 等 Buffer 与 Framebuffer、Pipeline 等能力。
- 生命周期清晰：资源创建由 Device 统一负责，绑定由 Context 负责。

## 设计原则

- 公共接口使用引擎自有的枚举/结构体；各后端自行翻译为原生 API。
- 资源对象本身不提供 Bind/Unbind；绑定是 Context 的职责。
- OpenGL 的 VAO 仅在 GL 后端内部使用；公共接口使用 VertexInputLayout 概念。
- Device 负责创建资源与能力查询；Context 负责命令/绑定/绘制；避免全局状态。

## 总体抽象

- Device：资源工厂与能力查询（capabilities）。
- Context：命令编码/执行（绑定、更新、绘制）。
- Buffer：GPU 连续内存资源（顶点、索引、常量、存储、间接、暂存等）。
- VertexInputLayout：顶点输入布局的描述；GL 后端可用 VAO 内部实现，DX/VK 用各自输入布局/状态。

## 核心枚举与结构（公共，不依赖后端）

- BufferKind：Vertex, Index, Constant, Storage, Indirect, Staging
- BufferUsage：Default, Immutable, Dynamic, Staging
- CpuAccessFlags（位掩码）：None, Read, Write
- BindFlags（位掩码）：VertexBuffer, IndexBuffer, ConstantBuffer, ShaderResource, UnorderedAccess, Indirect
- IndexFormat：UInt16, UInt32
- DataFormat（顶点属性格式）：R32_Float, R32G32_Float, R32G32B32_Float, R32G32B32A32_Float, R8G8B8A8_UNorm, ...（可按需扩展）
- BufferDesc：
  - size：size_t
  - kind：BufferKind
  - usage：BufferUsage
  - bindFlags：BindFlags
  - cpuAccess：CpuAccessFlags
  - stride：可选（结构化缓冲每元素大小）
  - indexFormat：索引缓冲专用（可选）
  - name：调试名（可选）
- SubresourceData：
  - data：const void*
  - size：size_t
  - offset：size_t（默认 0）
- MapMode：Read, Write, WriteDiscard, WriteNoOverwrite

## 公共接口（草案）

- class IBuffer
  - const BufferDesc& GetDesc() const
  - void Update(const SubresourceData& src) // 支持 offset + size 的局部更新
  - void* Map(MapMode mode)
  - void Unmap()

- struct VertexAttribute
  - location（或 semantic）、format（DataFormat）、offset（字节）、bindingSlot、stepRate（1：每顶点；>1：实例步进）

- class IVertexInputLayout
  - 由若干 VertexAttribute 构建

- class IDevice
  - std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc&, const SubresourceData* initial = nullptr)
  - std::shared_ptr<IVertexInputLayout> CreateVertexInputLayout(const VertexAttribute* attrs, uint32_t count)
  - 能力查询（如 MinConstantBufferAlignment()）

- class IContext
  - void SetVertexBuffers(uint32_t startSlot, span<IBuffer*>, span<uint32_t> strides, span<uint32_t> offsets)
  - void SetIndexBuffer(IBuffer*, IndexFormat)
  - void SetConstantBuffer(uint32_t stage, uint32_t slot, IBuffer*)
  - void SetVertexInputLayout(IVertexInputLayout*)
  - void Draw(uint32_t vertexCount, uint32_t startVertex)
  - void DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex)

注：以上为接口草图，命名/命名空间可按项目风格微调。

## 为什么 VAO 不应成为 Buffer

- VAO 是 OpenGL 特有的“输入状态对象”，并非数据资源；DX 用的是输入布局 + 顶点缓冲，VK 用顶点输入状态。
- 因此公共接口使用 VertexInputLayout 来描述输入属性；OpenGL 后端内部用 VAO 来实现这一概念。

## 后端映射

### OpenGL

- BufferKind → GL 目标：
  - Vertex → GL_ARRAY_BUFFER
  - Index → GL_ELEMENT_ARRAY_BUFFER
  - Constant → GL_UNIFORM_BUFFER
  - Storage → GL_SHADER_STORAGE_BUFFER
  - Indirect → GL_DRAW_INDIRECT_BUFFER
- Usage/CpuAccess → GL 使用/映射策略：
  - Immutable → glBufferData（不 Map）
  - Dynamic → GL_DYNAMIC_DRAW + glMapBufferRange（写入为主，优先 WriteDiscard）
  - Staging → 使用 COPY_READ/WRITE + Map
- VertexInputLayout → 后端内部 VAO（可做缓存），或在绑定时调用 glVertexAttribPointer。
- DataFormat → （GL 类型 + 分量数/是否归一化）的组合。
- 常量缓冲对齐 → 查询 GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT，在 Device 暴露最小对齐。

GL 头文件与包含顺序注意：
- 通过编译宏定义 GLFW_INCLUDE_NONE（在 CMake target_compile_definitions 上设置），阻止 GLFW 自动包含系统 GL 头。
- 在源文件中先包含 <glad/gl.h>，再包含任何 GLFW 相关头（如需要）。

### Direct3D（11/12）— 概览

- Usage/CpuAccess 映射到 D3D 的 usage 与 CPU 访问标志。
- VertexInputLayout → D3D_INPUT_ELEMENT_DESC。
- 常量缓冲对齐 16 字节；由 Device 暴露并在调用方校验。
- 动态更新：D3D11 用 Map(WRITE_DISCARD/NO_OVERWRITE)；D3D12 可用上传堆或 ring buffer。

### Vulkan — 概览

- Buffer 用途映射到 VkBufferUsageFlags。
- 内存分配：初期可一资源一分配，后续接入 VMA。
- 顶点输入布局 → VkVertexInput* 结构。
- 更新：常用 staging + vkCmdCopyBuffer；或 host-visible 内存并正确同步。

## 使用示例（引擎侧）

```cpp
// 顶点缓冲
BufferDesc vb{};
vb.size = sizeof(Vertex) * vertexCount;
vb.kind = BufferKind::Vertex;
vb.usage = BufferUsage::Immutable;
vb.bindFlags = BindFlags::VertexBuffer;
auto vertexBuffer = device->CreateBuffer(vb, SubresourceData{vertices, vb.size, 0});

// 索引缓冲
BufferDesc ib{};
ib.size = sizeof(uint32_t) * indexCount;
ib.kind = BufferKind::Index;
ib.usage = BufferUsage::Immutable;
ib.bindFlags = BindFlags::IndexBuffer;
ib.indexFormat = IndexFormat::UInt32;
auto indexBuffer = device->CreateBuffer(ib, SubresourceData{indices, ib.size, 0});

// 顶点输入布局
VertexAttribute attrs[] = {
  { .location=0, .format=DataFormat::R32G32B32_Float, .offset=0,  .bindingSlot=0, .stepRate=1 },
  { .location=1, .format=DataFormat::R32G32_Float,    .offset=12, .bindingSlot=0, .stepRate=1 },
};
auto layout = device->CreateVertexInputLayout(attrs, 2);

// 绑定与绘制
context->SetVertexBuffers(0, {vertexBuffer.get()}, {sizeof(Vertex)}, {0});
context->SetIndexBuffer(indexBuffer.get(), IndexFormat::UInt32);
context->SetVertexInputLayout(layout.get());
context->DrawIndexed(indexCount, 0, 0);
```

## 生命周期、所有权与线程

- Device 创建资源，默认以 shared_ptr 返回（简化所有权）。Context 在一次命令提交范围内持有非拥有指针。
- 现阶段可使用单一立即上下文；后续如需多线程录制，可引入命令列表与同步原语。
- 销毁建议以 RAII 为主；后端内部可提供 Destroy，但公共接口不必暴露。

## 错误处理

- CreateBuffer / CreateVertexInputLayout 失败返回 nullptr，并记录错误日志。
- Map/Update 越界或非法参数：Debug 构建断言，Release 记录错误并尽量无害返回。

## 迁移计划（从当前代码）

1. 从公共 API 中移除 BufferType::VAO/FBO；Framebuffer 后续以 IFramebuffer/RenderPass 单独文档与实现。
2. 在 Engine/Renderer/Gfx 下新增抽象头：IBuffer / IVertexInputLayout / IDevice / IContext 与相关枚举结构。
3. 实现 OpenGL 最小后端：
   - GLBuffer（实现 IBuffer）、GLVertexInputLayout（实现 IVertexInputLayout，内部用 VAO）、GLDevice、GLContext。
   - 保留现有 GLFW + glad 的窗口/上下文创建；将 Device/Context 接入现有 Renderer。
4. 引擎调用改为通过 Device 创建、Context 绑定；删去公共 Buffer::Bind/Unbind。
5. 在 CMake 中加入后端选择开关；默认 OpenGL。为相关 target 添加 GLFW_INCLUDE_NONE。

## 后续工作

- 若方案通过：
  - 搭建 Gfx 接口骨架（仅头文件）。
  - 落地 OpenGL 最小闭环（Buffer + VertexInputLayout + 绑定/Draw），并迁移一个示例验证。
  - 文档化如何在 CMake 中切换后端。

---
如果你对命名、接口粒度、或第一阶段交付范围有调整意见，告诉我，我再根据你的偏好微调后开始实现。 
