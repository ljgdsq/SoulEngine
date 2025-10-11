# Example2 - 高级示例
cmake_minimum_required(VERSION 3.14)

# 定义可执行文件
add_executable(Example2 main.cpp)

# 设置C++标准
set_property(TARGET Example2 PROPERTY CXX_STANDARD 17)

# 链接引擎库
target_link_libraries(Example2 PRIVATE SoulEngine)

# 链接第三方库
target_link_libraries(Example2 PRIVATE spdlog::spdlog)

# 设置输出目录
if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(Example2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(Example2 PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()

# 设置IDE中的文件夹
set_target_properties(Example2 PROPERTIES FOLDER "Launcher/Examples")