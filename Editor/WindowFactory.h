#pragma once
#include <functional>
#include <memory>
#include <string>
#include "GUI/GuiWindow.h"

namespace SoulEditor
{
    class GuiWindow;

    class WindowFactory
    {
    public:
        using WindowCreator = std::function<std::shared_ptr<GuiWindow>()>;

        static WindowFactory& GetInstance()
        {
            static WindowFactory instance;
            return instance;
        }

        // 创建窗口
        std::shared_ptr<GuiWindow> CreateWindow(const std::string& name);

        // 注册窗口创建函数
        void RegisterWindow(const std::string& name, WindowCreator creator);

        // 检查窗口是否已注册
        bool IsRegistered(const std::string& name) const;

        // 获取所有已注册的窗口名称
        std::vector<std::string> GetRegisteredWindows() const;

    private:
        WindowFactory() = default;
        std::unordered_map<std::string, WindowCreator> creators_;
    };
}
