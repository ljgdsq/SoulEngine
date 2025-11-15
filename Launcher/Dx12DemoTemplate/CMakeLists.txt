# DXDemo - 基础示例
cmake_minimum_required(VERSION 3.14)

# 定义可执行文件（Windows 子系统，入口为 WinMain）
add_executable(DX12Demo WIN32 main.cpp)

# 设置C++标准
set_property(TARGET DX12Demo PROPERTY CXX_STANDARD 17)

# 链接引擎库
target_link_libraries(DX12Demo PRIVATE SoulEngine)

# 链接第三方库
target_link_libraries(DX12Demo PRIVATE spdlog::spdlog)

# 设置输出目录
if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(DX12Demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(DX12Demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()
target_link_libraries(DX12Demo )

# 设置IDE中的文件夹
set_target_properties(DX12Demo PROPERTIES FOLDER "Launcher/Examples")


set(DX12DEMO_SHADER ${CMAKE_SOURCE_DIR}/Launcher/DX12Demo/Triangle.hlsl)

add_custom_command(
    TARGET DX12Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DX12DEMO_SHADER}
            $<TARGET_FILE_DIR:DX12Demo>
)


target_include_directories(DX12Demo PRIVATE ${CMAKE_BINARY_DIR})

set(TARGET_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/target_path.h.in
    ${CMAKE_BINARY_DIR}/target_path.h
    @ONLY
)