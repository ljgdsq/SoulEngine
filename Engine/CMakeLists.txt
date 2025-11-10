# Engine模块 - SoulEngine核心
cmake_minimum_required(VERSION 3.14)

# 设置当前模块的文件夹名称
set(CMAKE_FOLDER "Engine")

# 收集所有引擎源文件
file(GLOB_RECURSE ENGINE_SOURCES
    "*.h"
    "*.hpp"
    "*.inl"
    "Core/*.cpp"
    "Core/*.h"
    "Renderer/*.cpp" 
    "Renderer/*.h"
    "Component/*.cpp" 
    "Component/*.h"
    "Physics/*.cpp"
    "Physics/*.h"
    "Audio/*.cpp"
    "Audio/*.h"
    "Log/*.cpp"
    "Log/*.h"
    "Window/*.cpp"
    "Window/*.h"
)

# 收集头文件用于IDE显示
file(GLOB_RECURSE ENGINE_HEADERS
    "*.h"
    "*.hpp"
    "*.inl"
    "Core/*.h"
    "Core/*.hpp"
    "Core/*.inl"
    "Renderer/*.h"
    "Renderer/*.hpp" 
    "Physics/*.h"
    "Physics/*.hpp"
    "Audio/*.h"
    "Audio/*.hpp"
    "Log/*.h"
    "Log/*.hpp"
    "Window/*.h"
    "Window/*.hpp"
)


if (SOULENGINE_WITH_OPENGL)
    file(GLOB_RECURSE ENGINE_GL_SOURCES "Renderer/OpenGL/*.cpp" "Renderer/OpenGL/*.h")
    list(APPEND ENGINE_SOURCES ${ENGINE_GL_SOURCES})
    list(APPEND ENGINE_HEADERS ${ENGINE_GL_SOURCES})
endif()

# 创建SoulEngine静态库
add_library(SoulEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# 设置C++标准
set_property(TARGET SoulEngine PROPERTY CXX_STANDARD 17)

# 设置包含目录
target_include_directories(SoulEngine 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/Component
        ${CMAKE_CURRENT_SOURCE_DIR}/Physics
        ${CMAKE_CURRENT_SOURCE_DIR}/Log
        ${CMAKE_CURRENT_SOURCE_DIR}/Audio
)

# 链接第三方库
target_link_libraries(SoulEngine PUBLIC spdlog::spdlog nlohmann_json::nlohmann_json)

# 当启用 OpenGL 后端时，向引擎目标添加宏与链接依赖
if (SOULENGINE_WITH_OPENGL)
    target_compile_definitions(SoulEngine PUBLIC SOULENGINE_ENABLE_OPENGL=1)
    # 链接抽象依赖目标，避免直接依赖第三方目标名
    if (TARGET SoulEngine::OpenGLDeps)
        target_link_libraries(SoulEngine PUBLIC SoulEngine::OpenGLDeps)
    else()
        message(WARNING "SoulEngine::OpenGLDeps not found; ensure root included OpenGLDeps.cmake")
    endif()
    # 可在此追加 OpenGL 加载库（如 glad/glew），当前仅使用系统提供
endif()

# 设置IDE中的文件夹结构
set_target_properties(SoulEngine PROPERTIES FOLDER "Engine")

# 为IDE组织源文件结构
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS})