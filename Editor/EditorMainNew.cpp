#include "SoulEngine.h"
#include "Core/ApplicationHelper.h"
#include "ProjectManager.h"
#include "WelcomeUI.h"
#include "MainEditorUI.h"
#include "ImGuiManager.h"
#include "Window/WindowSystem.h"
#include "ImGuiFileDialog.h"

using namespace SoulEngine;
using namespace SoulEngine::Gfx;
using namespace SoulEditor;

/**
 * @brief SoulEditor 主应用程序类
 */
class SoulEditorApplication : public SoulEngine::ApplicationBase
{
public:
    SoulEditorApplication() : ApplicationBase("SoulEditor")
    {
    }

private:
    // UI管理器
    std::unique_ptr<ImGuiManager> imguiManager_;
    std::unique_ptr<WelcomeUI> welcomeUI_;
    std::unique_ptr<MainEditorUI> mainEditorUI_;
    
    // 渲染资源（保留原有的渲染相关变量）
    std::shared_ptr<IProgram> program_;
    std::shared_ptr<IVertexInputLayout> layout_;
    std::shared_ptr<IBuffer> vertexBuffer1_;
    std::shared_ptr<IBuffer> vertexBuffer2_;
    std::shared_ptr<IBuffer> indexBuffer_;

public:
    bool OnInitialize() override
    {
        Logger::Log("SoulEditor initialized");
        
        // 设置窗口不可resize（启动时）
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), false);
        
        // 初始化ImGui管理器
        imguiManager_ = std::make_unique<ImGuiManager>();
        if (!imguiManager_->Initialize(GetWindow()->GetGLFWWindow()))
        {
            Logger::Error("Failed to initialize ImGui manager");
            return false;
        }
        
        // 初始化UI管理器
        welcomeUI_ = std::make_unique<WelcomeUI>();
        mainEditorUI_ = std::make_unique<MainEditorUI>();
        
        // 设置项目管理器回调
        auto& projectManager = ProjectManager::GetInstance();
        projectManager.SetOnProjectOpenedCallback([this](const std::string& projectPath) {
            OnProjectOpened(projectPath);
        });
        projectManager.SetOnProjectClosedCallback([this]() {
            OnProjectClosed();
        });
        
        // 设置UI回调
        welcomeUI_->SetOnCreateProjectCallback([this]() {
            ImGuiFileDialog::Instance()->OpenDialog("CreateProjectDlg", "Select Project Folder", ".");
        });
        welcomeUI_->SetOnOpenProjectCallback([this]() {
            ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
        });
        
        mainEditorUI_->SetOnCloseProjectCallback([this]() {
            ProjectManager::GetInstance().CloseProject();
        });
        
        // 加载最近项目
        projectManager.LoadRecentProjects();
        
        return true;
    }

    void Update(float deltaTime) override
    {
        // 更新逻辑
    }
    
    void Render() override
    {
        // 开始ImGui帧
        imguiManager_->BeginFrame();
        
        // 根据项目状态动态控制Docking
        bool projectOpened = ProjectManager::GetInstance().IsProjectOpened();
        if (projectOpened && !imguiManager_->IsDockingEnabled())
        {
            imguiManager_->EnableDocking(true);
        }
        else if (!projectOpened && imguiManager_->IsDockingEnabled())
        {
            imguiManager_->EnableDocking(false);
        }
        
        // 创建DockSpace（如果需要）
        if (projectOpened)
        {
            imguiManager_->CreateDockSpace();
        }
        
        // 渲染UI
        if (projectOpened)
        {
            mainEditorUI_->Render();
        }
        else
        {
            welcomeUI_->Render();
        }
        
        // 结束ImGui帧
        imguiManager_->EndFrame();
    }

    void Shutdown() override
    {
        // 保存最近项目
        ProjectManager::GetInstance().SaveRecentProjects();
        
        // 清理UI管理器
        mainEditorUI_.reset();
        welcomeUI_.reset();
        
        // 清理ImGui管理器
        if (imguiManager_)
        {
            imguiManager_->Shutdown();
            imguiManager_.reset();
        }
        
        Logger::Log("SoulEditor shutdown");
    }

    bool ShouldClose() const override
    {
        return GetWindow()->ShouldClose();
    }
    
private:
    void OnProjectOpened(const std::string& projectPath)
    {
        Logger::Log("Project opened: {}", projectPath);
        
        // 启用窗口resize功能
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), true);
        
        // 可以在这里添加项目打开后的初始化逻辑
        // 比如加载项目配置、初始化资源管理器等
    }
    
    void OnProjectClosed()
    {
        Logger::Log("Project closed, returning to welcome screen");
        
        // 禁用窗口resize功能
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), false);
        
        // 可以在这里添加项目关闭后的清理逻辑
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(SoulEditorApplication)