# DXDemo - 基础示例
cmake_minimum_required(VERSION 3.14)

add_executable(DX12App WIN32 main.cpp)

set_property(TARGET DX12App PROPERTY CXX_STANDARD 17)

target_link_libraries(DX12App PRIVATE SoulEngine)

target_link_libraries(DX12App PRIVATE spdlog::spdlog)

if (CMAKE_CONFIGURATION_TYPES)
    set_target_properties(DX12App PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/Examples
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/Examples
    )
else()
    set_target_properties(DX12App PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Examples)
endif()
target_link_libraries(DX12App )

set_target_properties(DX12App PROPERTIES FOLDER "Launcher/Examples")

target_include_directories(DX12App PRIVATE ${CMAKE_BINARY_DIR})

set(TARGET_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/target_path.h.in
    ${CMAKE_BINARY_DIR}/target_path.h
    @ONLY
)