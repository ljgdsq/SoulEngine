# Launcher模块 - 包含各种执行样例
cmake_minimum_required(VERSION 3.14)

# 设置当前模块的文件夹名称
set(CMAKE_FOLDER "Launcher")

# 添加各个样例子目录

if(WIN32)
    add_subdirectory(DXDemo)
    add_subdirectory(DX12Demo)
    add_subdirectory(DX12App)
endif(WIN32)
add_subdirectory(OpenGLDemo)

# 可以继续添加更多样例
# add_subdirectory(ExampleAdvanced)