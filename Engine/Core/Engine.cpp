#include "Engine.h"
#include "Application.h"
#include <iostream>
#include <chrono>

namespace SoulEngine {
    
    Engine::Engine() {
        spdlog::info("SoulEngine created");
    }
    
    Engine::~Engine() {
        if (m_initialized) {
            Shutdown();
        }
        spdlog::info("SoulEngine destroyed");
    }
    
    bool Engine::Initialize() {
        if (m_initialized) {
            spdlog::warn("Engine already initialized");
            return true;
        }
        
        spdlog::info("Initializing SoulEngine...");
        
        // 初始化各个子系统
        // TODO: 初始化渲染器
        // TODO: 初始化物理系统
        // TODO: 初始化音频系统
        
        m_initialized = true;
        spdlog::info("SoulEngine initialized successfully");
        return true;
    }
    
    void Engine::Shutdown() {
        if (!m_initialized) {
            return;
        }
        
        spdlog::info("Shutting down SoulEngine...");
        
        // 关闭应用程序
        if (m_application) {
            m_application->Shutdown();
            m_application.reset();
        }
        
        // 关闭各个子系统
        // TODO: 关闭音频系统
        // TODO: 关闭物理系统  
        // TODO: 关闭渲染器
        
        m_isRunning = false;
        m_initialized = false;
        
        spdlog::info("SoulEngine shutdown complete");
    }
    
    int Engine::Run(std::unique_ptr<Application> app) {
        if (!m_initialized) {
            spdlog::error("Engine must be initialized before running");
            return -1;
        }
        
        if (!app) {
            spdlog::error("Application cannot be null");
            return -1;
        }
        
        m_application = std::move(app);
        
        if (!m_application->Initialize()) {
            spdlog::error("Failed to initialize application");
            return -1;
        }
        
        spdlog::info("Starting main loop...");
        m_isRunning = true;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // 主循环
        while (m_isRunning) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // 更新应用程序
            if (!m_application->Update(deltaTime)) {
                break;
            }
            
            // 渲染
            m_application->Render();
            
            // 检查退出条件
            if (m_application->ShouldClose()) {
                break;
            }
        }
        
        spdlog::info("Main loop ended");
        return 0;
    }
    
    Engine& Engine::GetInstance() {
        static Engine instance;
        return instance;
    }
    
} // namespace SoulEngine