# SoulEngine 项目结构总结

## 项目目录结构

```
SoulEngine/
├── CMakeLists.txt          # 主CMake配置文件
├── Engine/                 # 引擎核心模块
│   └── CMakeLists.txt      # 引擎模块配置
├── Launcher/               # 样例集合模块
│   ├── CMakeLists.txt      # Launcher模块配置
│   ├── README.md           # 样例集成指南
│   ├── Example1/           # 基础样例
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── Example2/           # 场景管理样例
│       ├── CMakeLists.txt
│       └── main.cpp
├── Editor/                 # 编辑器模块
│   └── CMakeLists.txt      # 编辑器模块配置
├── Test/                   # 测试模块
├── ThirdParty/             # 第三方库
└── build/                  # 构建输出目录
    ├── Debug/Examples/     # Debug版本样例输出
    └── Release/Examples/   # Release版本样例输出
```

## 构建系统集成

### 主CMakeLists.txt特性
- 使用现代CMake (3.14+)
- 自动集成第三方库 (spdlog, GoogleTest)
- 支持多配置生成器 (Visual Studio)
- 模块化设计，各模块独立

### Launcher模块集成方式
- 每个样例都是独立的可执行文件
- 统一的输出目录管理
- 共享的依赖管理 (spdlog等)
- IDE中的文件夹组织

## 样例程序

### Example1 - 基础样例
- 演示基本的程序结构
- 日志系统使用
- 简单的主循环框架
- 输出: `Example1.exe`

### Example2 - 场景管理样例  
- 面向对象的场景管理
- 智能指针使用示例
- 更复杂的游戏循环模拟
- 输出: `Example2.exe`

## 构建命令

### 配置项目
```bash
cmake -B build -S .
```

### 编译项目
```bash
# Release版本
cmake --build build --config Release

# Debug版本  
cmake --build build --config Debug
```

### 运行样例
```bash
# Example1
.\build\Release\Examples\Example1.exe

# Example2
.\build\Release\Examples\Example2.exe
```

## 添加新样例的步骤

1. 在 `Launcher/` 下创建新目录 (如 `Example3/`)
2. 复制现有样例的 `CMakeLists.txt` 并修改目标名
3. 创建你的 `main.cpp` 文件
4. 在 `Launcher/CMakeLists.txt` 中添加 `add_subdirectory(Example3)`
5. 重新配置和构建项目

## 扩展性设计

### 引擎模块扩展
- `Engine/Core/` - 核心功能
- `Engine/Renderer/` - 渲染系统  
- `Engine/Physics/` - 物理系统
- 等等...

### 样例类型扩展
- 渲染示例 (OpenGL/DirectX)
- 物理模拟示例
- 音频处理示例
- 网络通信示例
- 等等...

### 构建配置扩展
- 支持不同的构建类型
- 可选的模块构建
- 平台特定的配置
- 第三方库的可选集成

## 当前状态

✅ 项目结构已搭建完成
✅ 基础构建系统已配置
✅ 两个示例程序已实现并测试通过
✅ 模块化设计已实现
✅ 第三方库集成已完成

## 下一步建议

1. 完善Engine模块的核心功能
2. 添加更多类型的样例程序
3. 集成图形渲染库 (OpenGL/DirectX)
4. 添加单元测试
5. 完善文档和注释