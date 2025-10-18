#include "Engine.h"
#include "Application.h"
#include <iostream>
#include "Timer.h"
namespace SoulEngine {
    
    Engine::Engine() {
        Logger::Log("SoulEngine created");
    }
    
    Engine::~Engine() {
        if (m_initialized) {
            Shutdown();
        }

        Logger::Log("SoulEngine destroyed");
    }
    
    bool Engine::Initialize() {
        if (m_initialized) {
            Logger::Warn("Engine already initialized");
            return true;
        }

        Logger::Log("Initializing SoulEngine...");
        Timer::GetInstance().Initialize();
        // 初始化渲染器

        
        // 初始化其他子系统
        // TODO: 初始化物理系统
        // TODO: 初始化音频系统
        
        m_initialized = true;
        Logger::Log("SoulEngine initialized successfully");
        return true;
    }
    
    void Engine::Shutdown() {
        if (!m_initialized) {
            return;
        }

        Logger::Log("Shutting down SoulEngine...");

        // 关闭应用程序
        if (m_application) {
            m_application->Shutdown();
            m_application.reset();
        }
        
        // 关闭渲染器
 
        
        // 关闭各个子系统
        // TODO: 关闭音频系统
        // TODO: 关闭物理系统  
        
        m_initialized = false;

        Logger::Log("SoulEngine shutdown complete");
    }
    
    int Engine::Run(std::unique_ptr<Application> app) {
        if (!m_initialized) {
            Logger::Error("Engine must be initialized before running");
            return -1;
        }
        
        if (!app) {
            Logger::Error("Application cannot be null");
            return -1;
        }
        
        m_application = std::move(app);
        
        if (!m_application->Initialize()) {
            Logger::Error("Failed to initialize application");
            return -1;
        }

        Logger::Log("Starting main loop...");
        
        auto timer = &Timer::GetInstance();
        // 主循环
        while (!m_application->ShouldClose()) {
            timer->Update();

            // 更新应用程序
            m_application->Update(timer->GetDeltaTime());
            
            // 渲染
            m_application->Render();
         
        }
        
        Logger::Log("Main loop ended");
        return 0;
    }
    
} // namespace SoulEngine