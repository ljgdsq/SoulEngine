#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <concepts>
#include "EngineSystem.h"
#include "ServiceRegistry.h"

namespace SoulEngine
{

    class Application;

    /**
     * @brief 引擎主类 - 管理整个引擎的生命周期
     */
    class Engine
    {
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
        static Engine &GetInstance();

        /**
         * @brief 检查引擎是否正在运行
         */
        bool IsRunning() const { return m_isRunning; }

        /**
         * @brief 通过服务注册表获取某个接口实例（如 Renderer）
         */
        template <class T>
        T* GetService() const { return m_services.Get<T>(); }

        /**
         * @brief 便捷方法：获取当前渲染器接口
         */
        class Renderer* GetRenderer() const;

        /**
         * @brief 由系统在初始化/关闭时注册/反注册接口实例
         */
        template <class T>
            requires std::derived_from<T, IEngineSystem>
        void RegisterService(T* instance) { m_services.Register(std::type_index(typeid(T)), instance); }
        template <class T>
            requires std::derived_from<T, IEngineSystem>
        void UnregisterService(T* instance) { m_services.Unregister(std::type_index(typeid(T)), instance); }

    private:
        bool m_isRunning = false;
        bool m_initialized = false;
        std::unique_ptr<Application> m_application;
        ServiceRegistry m_services;

        // 禁用拷贝
        Engine(const Engine &) = delete;
        Engine &operator=(const Engine &) = delete;
    };

} // namespace SoulEngine