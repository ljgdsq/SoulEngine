project(imgui)

file(GLOB IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

if(SOULENGINE_WITH_OPENGL)
    file(GLOB_RECURSE IMGUI_GL_SOURCES "backends/imgui_impl_glfw.cpp" "backends/imgui_impl_opengl3.cpp")
    list(APPEND IMGUI_SOURCES ${IMGUI_GL_SOURCES})
endif()

if(SOULENGINE_WITH_DX11)
    file(GLOB_RECURSE IMGUI_DX11_SOURCES "backends/imgui_impl_dx11.cpp" "backends/imgui_impl_win32.cpp")
    list(APPEND IMGUI_SOURCES ${IMGUI_DX11_SOURCES})
endif()

if(SOULENGINE_WITH_DX12)
    file(GLOB_RECURSE IMGUI_DX12_SOURCES "backends/imgui_impl_dx12.cpp" "backends/imgui_impl_win32.cpp")
    list(APPEND IMGUI_SOURCES ${IMGUI_DX12_SOURCES})
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})

# 设置include目录
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)


if(SOULENGINE_WITH_OPENGL)
    # 查找OpenGL
    find_package(OpenGL REQUIRED)

    # 链接OpenGL
    target_link_libraries(imgui PUBLIC
        OpenGL::GL
    )

    # 如果项目中已有glfw，链接到glfw
    if(TARGET glfw)
        target_link_libraries(imgui PUBLIC glfw)
    endif()

endif()

if(SOULENGINE_WITH_DX11)
    # # 链接DirectX 11库
    # target_link_libraries(imgui PUBLIC
    #     d3d11
    #     dxgi
    #     d3dcompiler
    #     user32
    #     gdi32
    # )
endif()