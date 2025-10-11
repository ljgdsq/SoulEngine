# Engine模块 - SoulEngine核心
cmake_minimum_required(VERSION 3.14)

# 设置当前模块的文件夹名称
set(CMAKE_FOLDER "Engine")

# 收集所有引擎源文件
file(GLOB_RECURSE ENGINE_SOURCES
    "Core/*.cpp"
    "Core/*.h"
    "Renderer/*.cpp" 
    "Renderer/*.h"
    "Physics/*.cpp"
    "Physics/*.h"
    "Audio/*.cpp"
    "Audio/*.h"
)

# 收集头文件用于IDE显示
file(GLOB_RECURSE ENGINE_HEADERS
    "Core/*.h"
    "Core/*.hpp"
    "Renderer/*.h"
    "Renderer/*.hpp" 
    "Physics/*.h"
    "Physics/*.hpp"
    "Audio/*.h"
    "Audio/*.hpp"
)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/Physics
        ${CMAKE_CURRENT_SOURCE_DIR}/Audio
)

# 链接第三方库
target_link_libraries(SoulEngine PUBLIC spdlog::spdlog)

# 设置IDE中的文件夹结构
set_target_properties(SoulEngine PROPERTIES FOLDER "Engine")

# 为IDE组织源文件结构
source_group("Core" FILES ${ENGINE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS})