#include "WindowSystem.h"
#if defined(SOULENGINE_ENABLE_OPENGL)
#include "Window/Platform/GLFWWindow.h"
#endif


namespace SoulEngine
{

    int WindowSystem::GetPriority() const
    {
        return 5; // 很早初始化
    }

    bool WindowSystem::Initialize()
    {
        Logger::Log("Initializing WindowSystem...");
        // 在这里初始化窗口系统（例如GLFW）
        #if defined(SOULENGINE_ENABLE_OPENGL)
        m_window = new GLFWWindow();
        #endif
        if (m_window)
        {
            WindowConfig config;
            config.width = 800;
            config.height = 600;
            config.title = "SoulEngine Window";
            m_window->Initialize(config);
            Logger::Log("WindowSystem initialized successfully");
        }
        else
        {
            Logger::Error("Failed to create window");
            return false;
        }
    }

    void WindowSystem::Update(float dt)
    {
        // 在这里处理窗口事件
    }

    void WindowSystem::Shutdown()
    {
       Logger::Log("Shutting down WindowSystem...");
        // 在这里关闭窗口系统

        
    }

}