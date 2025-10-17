
# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.14)

# 定义项目名称和使用的语言
project(SoulEngine LANGUAGES CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 设置C++标准为C++20
set(CMAKE_CXX_STANDARD 20)                # 使用C++20标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)       # 必须使用指定的C++标准
set(CMAKE_CXX_EXTENSIONS OFF)             # 禁用编译器特定扩展

# Windows/MSVC: 将源文件按UTF-8处理，消除中文注释导致的C4819告警
if (MSVC)
  add_compile_options(/utf-8)
endif()

# 集成第三方库
include(FetchContent)                     # 引入FetchContent模块用于下载依赖

set(CMAKE_FOLDER "thirdlib") # 这会影响所有接下来引入的目标

# 图形后端可选项（默认均关闭以保持最小依赖）
option(SOULENGINE_WITH_OPENGL "Enable OpenGL backend" OFF)
option(SOULENGINE_WITH_DX12   "Enable DirectX 12 backend" OFF)
option(SOULENGINE_WITH_VULKAN "Enable Vulkan backend" OFF)

# 当启用 OpenGL 后端时，纳入独立依赖模块（优先 find_package，必要时 FetchContent）
if (SOULENGINE_WITH_OPENGL)
  message(STATUS "SoulEngine: OpenGL backend ENABLED")
  include(${CMAKE_SOURCE_DIR}/cmake/backends/OpenGLDeps.cmake)
endif()

message(STATUS "SoulEngine load cmake")

# 添加spdlog日志库
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(spdlog)


    message(STATUS "SoulEngine: fetching glfw via FetchContent")
    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG 3.4
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS    OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL       OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)


# 如果启用测试，添加GoogleTest
if(USE_TEST)
    FetchContent_Declare(
      googletest                              # 声明GoogleTest依赖
      URL https://github.com/google/googletest/archive/refs/heads/main.zip # 下载地址
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # Windows下强制使用父项目的编译器/链接器设置，避免冲突
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)    # 下载并添加GoogleTest
endif(USE_TEST)

set(CMAKE_FOLDER "") # 重置

# 添加各个模块
add_subdirectory(Engine)
add_subdirectory(Launcher)
add_subdirectory(Editor)

if(USE_TEST)
        # 启用测试功能
    enable_testing()                          # 允许添加和运行测试

    # 添加测试可执行文件
    # add_executable(test_hello tests/test_hello.cpp) # 生成test_hello.exe，源码在tests/test_hello.cpp
    # target_link_libraries(test_hello PRIVATE gtest_main spdlog::spdlog) # 链接GoogleTest主库
    # target_link_libraries(hello PRIVATE spdlog::spdlog)
    # 设置测试程序的输出目录，兼容多配置生成器
    # if (CMAKE_CONFIGURATION_TYPES)
    # set_target_properties(test_hello PROPERTIES
    #     RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug      # Debug模式输出路径
    #     RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release    # Release模式输出路径
    # )
    # else()
    # set_target_properties(test_hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) # 单配置生成器输出路径
    # endif()

    # set_target_properties(spdlog PROPERTIES FOLDER "thirdlib")

    # 注册测试，ctest会自动运行test_hello
    # add_test(NAME HelloTest COMMAND test_hello)
endif(USE_TEST)







