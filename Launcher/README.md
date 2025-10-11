# SoulEngine Launcher 样例集成指南

## 目录结构

```
Launcher/
├── CMakeLists.txt          # 主Launcher CMake文件
├── Example1/               # 基础示例
│   ├── CMakeLists.txt
│   └── main.cpp
├── Example2/               # 高级示例
│   ├── CMakeLists.txt
│   └── main.cpp
└── README.md               # 本文件
```

## 如何添加新的样例

### 步骤1：创建新目录
为新样例创建目录，例如 `Example3`：

```
mkdir Launcher/Example3
```

### 步骤2：创建CMakeLists.txt
在新目录中创建 `CMakeLists.txt` 文件：

```cmake
# Example3 - 你的示例描述
cmake_minimum_required(VERSION 3.14)

# 定义可执行文件
add_executable(Example3 main.cpp)

# 设置C++标准
set_property(TARGET Example3 PROPERTY CXX_STANDARD 17)

# 链接引擎库（当Engine模块完成后）
# target_link_libraries(Example3 PRIVATE SoulEngine)

# 链接第三方库
target_link_libraries(Example3 PRIVATE spdlog::spdlog)

# 设置输出目录
if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(Example3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(Example3 PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()

# 设置IDE中的文件夹
set_target_properties(Example3 PROPERTIES FOLDER "Launcher/Examples")
```

### 步骤3：创建main.cpp
在新目录中创建你的主程序文件。

### 步骤4：注册到主CMakeLists.txt
在 `Launcher/CMakeLists.txt` 中添加新的子目录：

```cmake
add_subdirectory(Example3)
```

## 构建和运行

### 配置项目
```bash
cmake -B build -S .
```

### 编译
```bash
cmake --build build --config Release
```

### 运行样例
编译后的可执行文件会在以下位置：
- Debug模式: `build/Debug/Examples/`
- Release模式: `build/Release/Examples/`

## 样例说明

### Example1 - 基础示例
- 演示基本的引擎初始化流程
- 简单的主循环结构
- 日志系统使用示例

### Example2 - 场景管理示例
- 演示面向对象的场景管理
- 智能指针的使用
- 更复杂的游戏循环模拟

## 注意事项

1. 每个样例都是独立的可执行文件
2. 所有样例共享相同的输出目录设置
3. 在IDE中，样例会被组织在 "Launcher/Examples" 文件夹下
4. 当Engine模块完成后，记得取消注释引擎库的链接
5. 可以根据需要添加更多第三方库依赖