#pragma once
#include "GuiWindow.h"
namespace SoulEditor
{
    class ContentBrowser:public GuiWindow
    {
    public:
        ContentBrowser():GuiWindow("ContentBrowser"){}
        ~ContentBrowser() override = default;
        void OnInitialize() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnShutdown() override;

        void SetRootPath(const std::string& path) { rootPath = path; }
    protected:
        bool CanClose() const override;
    private:
        std::string rootPath;
        std::vector<std::string> currentPath;
    private:
        void RenderContent();
    };

    REGISTER_WINDOW(ContentBrowser);

}
