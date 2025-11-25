# DXDemo - 基础示例
cmake_minimum_required(VERSION 3.14)

add_executable(DX12Wave WIN32 main.cpp 
    d3dUtil.cpp
    D3DApp.cpp
    Camera.cpp
    DDSTextureLoader.cpp
    GameTimer.cpp
    GeometryGenerator.cpp
    MathHelper.cpp
    FrameResource.cpp
    Waves.cpp
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/color.hlsl PROPERTIES HEADER_FILE_ONLY TRUE)

if(MSVC)
    target_compile_options(DX12Wave PRIVATE /wd4355)
endif()

set_property(TARGET DX12Wave PROPERTY CXX_STANDARD 17)

target_link_libraries(DX12Wave PRIVATE SoulEngine)

target_link_libraries(DX12Wave PRIVATE spdlog::spdlog)

if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(DX12Wave PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(DX12Wave PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()
target_link_libraries(DX12Wave )

set_target_properties(DX12Wave PROPERTIES FOLDER "Launcher/Examples")

target_include_directories(DX12Wave PRIVATE ${CMAKE_BINARY_DIR})

set(TARGET_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/target_path.h.in
    ${CMAKE_BINARY_DIR}/target_path.h
    @ONLY
)

add_definitions(-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
target_compile_definitions(DX12Wave PRIVATE UNICODE _UNICODE)