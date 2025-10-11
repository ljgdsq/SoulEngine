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
        
    private:
        bool m_isRunning = false;
        bool m_initialized = false;
        std::unique_ptr<Application> m_application;
        
        // 禁用拷贝
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
    };
    
} // namespace SoulEngine