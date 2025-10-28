#pragma once

#include "imgui.h"
#include <functional>

namespace SoulEditor
{
    /**
     * @brief 欢迎界面UI管理器
     */
    class WelcomeUI
    {
    public:
        WelcomeUI();
        ~WelcomeUI() = default;
        
        // 渲染欢迎界面
        void Render();
        
        // 设置回调函数
        void SetOnCreateProjectCallback(std::function<void()> callback) { 
            onCreateProject_ = callback; 
        }
        void SetOnOpenProjectCallback(std::function<void()> callback) { 
            onOpenProject_ = callback; 
        }
        
        // 处理文件对话框
        void HandleFileDialogs();
        
    private:
        // 回调函数
        std::function<void()> onCreateProject_;
        std::function<void()> onOpenProject_;
        
        // UI渲染方法
        void RenderBackground();
        void RenderTitle();
        void RenderActionButtons();
        void RenderRecentProjects();
        
        // 文件对话框处理
        void HandleCreateProjectDialog();
        void HandleOpenProjectDialog();
        
        // UI样式设置
        void PushWelcomeStyles();
        void PopWelcomeStyles();
        
        // 样式计数器
        int styleColorCount_ = 0;
        int styleVarCount_ = 0;
    };
}