# Editor模块 - 编辑器工具
cmake_minimum_required(VERSION 3.14)

# 设置当前模块的文件夹名称
set(CMAKE_FOLDER "Editor")
set(_SE_PREV_CMAKE_FOLDER "${CMAKE_FOLDER}")
add_executable(Editor
    *.cpp
    # EditorApplication.cpp
    # 其他源文件...
)



set(CMAKE_FOLDER "thirdlib")

if(SOULENGINE_WITH_OPENGL)
    add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/imgui ${CMAKE_BINARY_DIR}/ThirdParty/imgui)
    add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/ImGuiFileDialog ${CMAKE_BINARY_DIR}/ThirdParty/ImGuiFileDialog)
    target_compile_definitions(Editor PRIVATE SOULENGINE_WITH_OPENGL)
    target_link_libraries(ImGuiFileDialog PRIVATE imgui)
    target_link_libraries(Editor PRIVATE SoulEngine::OpenGLDeps imgui ImGuiFileDialog SoulEngine)
else()
    target_link_libraries(Editor PRIVATE SoulEngine)
endif()

set(CMAKE_FOLDER "${CMAKE_FOLDER}")


