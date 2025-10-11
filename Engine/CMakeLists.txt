# Engine模块 - SoulEngine核心
cmake_minimum_required(VERSION 3.14)

# 设置当前模块的文件夹名称
set(CMAKE_FOLDER "Engine")

# 这里将来可以添加引擎的各个子模块
# add_subdirectory(Core)
# add_subdirectory(Renderer)
# add_subdirectory(Physics)

# 创建引擎库（当有源文件时取消注释）
# add_library(SoulEngine STATIC
#     Core/Engine.cpp
#     Core/Application.cpp
#     # 其他源文件...
# )
# 
# target_include_directories(SoulEngine PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
#
# target_link_libraries(SoulEngine PUBLIC spdlog::spdlog)