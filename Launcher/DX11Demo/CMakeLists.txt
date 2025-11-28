# DX11Demo - 基础示例
cmake_minimum_required(VERSION 3.14)


aux_source_directory(. DIR_SRCS)
file(GLOB HEADER_FILES ./*.h)


# 定义可执行文件（Windows 子系统，入口为 WinMain）
add_executable(DX11Demo WIN32 ${DIR_SRCS} ${HEADER_FILES})

add_compile_definitions(UNICODE _UNICODE)
if (WIN7_SYSTEM_SUPPORT MATCHES ON)
    add_compile_definitions(_WIN32_WINNT=0x601)
endif()
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# 设置C++标准
set_property(TARGET DX11Demo PROPERTY CXX_STANDARD 17)

# 链接引擎库
target_link_libraries(DX11Demo PRIVATE SoulEngine)

# 链接第三方库
target_link_libraries(DX11Demo PRIVATE spdlog::spdlog)

# 设置输出目录
if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(DX11Demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(DX11Demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()

target_link_libraries(DX11Demo PRIVATE d3d11.lib dxgi.lib dxguid.lib D3DCompiler.lib winmm.lib d2d1.lib dwrite.lib)

# 设置IDE中的文件夹
set_target_properties(DX11Demo PROPERTIES FOLDER "Launcher/Examples")

target_include_directories(DX11Demo PRIVATE ${CMAKE_BINARY_DIR})

set(TARGET_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/target_path.h.in
    ${CMAKE_BINARY_DIR}/target_path.h
    @ONLY
)