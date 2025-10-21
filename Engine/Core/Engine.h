#pragma once

#include <memory>
#include <string>
#include "Define.h"
#include <vector>

#include "Log/Logger.h"
#include "Core/SystemInterface.h"
namespace SoulEngine
{

    class Application;

    /**
     * @brief 引擎主类 - 管理整个引擎的生命周期
     */
    class Engine
    {
        NON_COPY_AND_MOVE(Engine)

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

        // Template system management functions are declared here and
        // implemented in Engine.inl (included at the end of this header).
        template <class T, class... Args>
        T* RegisterSystem(Args&&... args);

        template <class T>
        void UnregisterSystem();

        template <class T>
        T* GetSystem() const;

        void SortSystemsByPriority();


    private:
        bool m_initialized = false;
        std::unique_ptr<Application> m_application;
        std::vector<std::unique_ptr<SystemInterface>> m_systems;
    };

} // namespace SoulEngine

#include "Engine.inl"