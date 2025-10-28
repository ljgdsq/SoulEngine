#pragma once

#include "imgui.h"
#include "GLFW/glfw3.h"

namespace SoulEditor
{
    /**
     * @brief ImGui渲染管理器
     */
    class ImGuiManager
    {
    public:
        ImGuiManager() = default;
        ~ImGuiManager() = default;
        
        // 初始化和清理
        bool Initialize(GLFWwindow* window);
        void Shutdown();
        
        // 帧渲染管理
        void BeginFrame();
        void EndFrame();
        
        // Docking控制
        void EnableDocking(bool enable);
        bool IsDockingEnabled() const { return isDockingEnabled_; }
        
        // DockSpace管理
        void CreateDockSpace();
        
    private:
        bool isInitialized_ = false;
        bool isDockingEnabled_ = false;
        
        void SetupStyle();
    };
}