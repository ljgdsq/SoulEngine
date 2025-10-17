#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace SoulEngine {
    
    class Application;
    
    /**
     * @brief 引擎主类 - 管理整个引擎的生命周期
     */
    class Engine {
    public:
        Engine();
        ~Engine();
        
        /**
         * @brief 初始化引擎
         * @return true if successful
         */
        bool Initialize();
        
        /**
         * @brief 关闭引擎，释放资源
         */
        void Shutdown();
        
        /**
         * @brief 运行应用程序
         * @param app 应用程序实例
         * @return 退出代码
         */
        int Run(std::unique_ptr<Application> app);
        
        /**
         * @brief 获取引擎单例
         */
        static Engine& GetInstance();
        
        /**
         * @brief 检查引擎是否正在运行
         */
        bool IsRunning() const { return m_isRunning; }

    /**
     * @brief 获取当前渲染器（可能为空，取决于后端是否启用）
     */
    class Renderer* GetRenderer() const { return m_renderer.get(); }
        
    private:
    bool m_isRunning = false;
    bool m_initialized = false;
    std::unique_ptr<Application> m_application;
    std::unique_ptr<class Renderer> m_renderer; // 由RendererFactory创建
        
        // 禁用拷贝
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
    };
    
} // namespace SoulEngine