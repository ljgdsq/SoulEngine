#pragma once

#include "imgui.h"
#include <functional>

namespace SoulEditor
{
    /**
     * @brief 主编辑器UI管理器
     */
    class MainEditorUI
    {
    public:
        MainEditorUI();
        ~MainEditorUI() = default;
        
        // 渲染主编辑器界面
        void Render();
        
        // 面板显示控制
        void ShowPanel(const std::string& panelName, bool show = true);
        bool IsPanelVisible(const std::string& panelName) const;
        void ResetLayout();
        
        // 设置回调函数
        void SetOnCloseProjectCallback(std::function<void()> callback) { 
            onCloseProject_ = callback; 
        }
        
    private:
        // 面板显示状态
        bool showProjectBrowser_ = true;
        bool showSceneHierarchy_ = true;
        bool showInspector_ = true;
        bool showConsole_ = true;
        
        // 回调函数
        std::function<void()> onCloseProject_;
        
        // UI渲染方法
        void RenderMainMenuBar();
        void RenderProjectBrowser();
        void RenderSceneHierarchy();
        void RenderInspector();
        void RenderConsole();
        
        // 菜单处理
        void HandleFileMenu();
        void HandleViewMenu();
        void HandleHelpMenu();
        
        // 文件对话框处理
        void HandleFileDialogs();
    };
}