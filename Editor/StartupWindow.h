#pragma once
#include "GuiWindow.h"

namespace SoulEditor
{
    class StartupWindow:public GuiWindow
    {
    public:
        explicit StartupWindow()
            : GuiWindow("Startup")
        {
        }

        ~StartupWindow() override =default;
        void OnInitialize() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnShutdown() override;

    private:
        void RenderBackground();
        void RenderTitle();
        void RenderActionButtons();
        void RenderRecentProjects();

        // UI样式设置
        void PushWelcomeStyles();
        void PopWelcomeStyles();

        void HandleFileDialogs();
        void HandleCreateProjectDialog();
        void HandleOpenProjectDialog();

    };
    REGISTER_WINDOW(StartupWindow);
}

