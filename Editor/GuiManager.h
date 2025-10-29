#pragma once

#include <memory>

#include "imgui.h"
#include "GLFW/glfw3.h"
#include <vector>
#include "GuiWindow.h"
#include <unordered_map>

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

    private:
        bool initialized = false;
        std::vector<std::shared_ptr<GuiWindow>> m_windows;
        std::unordered_map<std::string,std::shared_ptr<GuiWindow>> m_windowMap;
    };
}

