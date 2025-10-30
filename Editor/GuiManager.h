#pragma once

#include <memory>

#include "imgui.h"
#include "GLFW/glfw3.h"
#include <vector>
#include "GuiWindow.h"

namespace SoulEditor
{
    class GuiManager
    {
    public:
        bool Initialize(GLFWwindow* window);
        void BeginFrame();
        void EndFrame();
        void Render() const;
        void Shutdown();
        void SetupStyle();

    public:
        std::shared_ptr<GuiWindow> OpenWindow(std::string name);
        template<class T>
        std::shared_ptr<T> OpenWindow(std::string name)
        {
            std::shared_ptr<GuiWindow> win= OpenWindow(name);
            return std::dynamic_pointer_cast<T>(win);
        }
        void CloseWindow(std::shared_ptr<GuiWindow> window);
        void CloseWindow(const std::string& name);
        std::shared_ptr<GuiWindow> GetWindow(const std::string& name) const;
    private:
        bool initialized = false;
        std::vector<std::shared_ptr<GuiWindow>> m_windows;
    public:
        void SwitchToMainEditor();
        void SwitchToStartupScreen();
    };
}

