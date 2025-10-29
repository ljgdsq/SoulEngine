#pragma once

#include <string>
#include "Window/IWindow.h"
namespace SoulEngine {
    
    /**
     * @brief 应用程序基类 - 用户需要继承此类来创建自己的应用
     */
    class Application {
    public:
        Application(const std::string& name = "SoulEngine Application");
        virtual ~Application() = default;
        
        /**
         * @brief 初始化应用程序 - 由 Engine 调用，会自动设置 engine 指针
         * @param engine 引擎实例指针
         * @return true if successful
         */
        bool Initialize(class Engine* engine) {
            this->engine = engine;
            return OnInitialize();
        }
        
        /**
         * @brief 用户需要重写的初始化方法
         * @return true if successful
         */
        virtual bool OnInitialize() = 0;
        
        /**
         * @brief 更新应用程序逻辑
         * @param deltaTime 上一帧到当前帧的时间间隔(秒)
         */
        virtual void Update(float deltaTime) = 0;
        
        /**
         * @brief 渲染应用程序
         */
        virtual void Render() = 0;
        
        /**
         * @brief 关闭应用程序
         */
        virtual void Shutdown() = 0;
        
        /**
         * @brief 检查应用程序是否应该关闭
         */
        virtual bool ShouldClose() const = 0;
        
        /**
         * @brief 获取应用程序名称
         */
        const std::string& GetName() const { return m_name; }
        
    protected:
        /**
         * @brief 获取引擎实例
         */
        class Engine* GetEngine() const { return engine; }
        
        /**
         * @brief 获取窗口实例
         */
        IWindow* GetWindow() const { return m_window; }
        
        std::string m_name;
        bool m_shouldClose = false;
        IWindow* m_window = nullptr;
        class Engine* engine = nullptr;
    public:
        void SetWindow(IWindow* window) { m_window = window; }
    };
    
} // namespace SoulEngine