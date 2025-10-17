# DXDemo - 基础示例
cmake_minimum_required(VERSION 3.14)

# 定义可执行文件（Windows 子系统，入口为 WinMain）
add_executable(DXDemo WIN32 main.cpp)

# 设置C++标准
set_property(TARGET DXDemo PROPERTY CXX_STANDARD 17)

# 链接引擎库
target_link_libraries(DXDemo PRIVATE SoulEngine)

# 链接第三方库
target_link_libraries(DXDemo PRIVATE spdlog::spdlog)

# 设置输出目录
if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(DXDemo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(DXDemo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()
target_link_libraries(DXDemo PRIVATE d3d11.lib dxgi.lib dxguid.lib D3DCompiler.lib winmm.lib d2d1.lib dwrite.lib)

# 设置IDE中的文件夹
set_target_properties(DXDemo PROPERTIES FOLDER "Launcher/Examples")