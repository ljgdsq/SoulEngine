#include "Engine.h"
#include "Application.h"
#include "Timer.h"
#include "Renderer/RenderSystem.h"
#include "Window/WindowSystem.h"
#include "Core/Input.h"

#include <iostream>

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
        
        // 初始化窗口系统
        RegisterSystem<WindowSystem>();
        
        // 初始化渲染器
        RegisterSystem<RenderSystem>();

        Input::GetInstance().Initialize(GetSystem<WindowSystem>()->GetWindow());

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
        UnregisterSystem<RenderSystem>();

        // 关闭窗口系统
        UnregisterSystem<WindowSystem>();
        
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
        m_application->SetWindow(GetSystem<WindowSystem>()->GetWindow());
        if (!m_application->Initialize()) {
            Logger::Error("Failed to initialize application");
            return -1;
        }
        SortSystemsByPriority();
        Logger::Log("Starting main loop...");
        
        auto timer = &Timer::GetInstance();
        auto renderer = GetSystem<RenderSystem>()->GetRenderer();
        auto window = GetSystem<WindowSystem>()->GetWindow();
        // 主循环
        while (!m_application->ShouldClose()) {
            timer->Update();
            window->PollEvents();
            Input::GetInstance().Update();
            auto deltaTime = timer->GetDeltaTime();
            // update system
            for(auto it =m_systems.begin();it!=m_systems.end();++it ){
                (*it)->Update(deltaTime);
            }
        
            // 更新应用程序
            m_application->Update(deltaTime);
            
            renderer->BeginFrame();
            renderer->Clear();
            m_application->Render();
            renderer->EndFrame();

            window->SwapBuffers();
        }
        
        Logger::Log("Main loop ended");
        return 0;
    }

    void Engine::SortSystemsByPriority()
    {
        std::sort(m_systems.begin(), m_systems.end(),
            [](const std::unique_ptr<SystemInterface>& a, const std::unique_ptr<SystemInterface>& b) {
                return a->GetPriority() < b->GetPriority();
            });
    }
    
} // namespace SoulEngine