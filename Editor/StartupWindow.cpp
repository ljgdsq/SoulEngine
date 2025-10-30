#include "StartupWindow.h"
#include "GuiManager.h"
#include "ImGuiFileDialog.h"
#include "Logger.h"
#include "Project/Project.h"
#include "Project/ProjectManager.h"
#include "Util/FileUtil.hpp"
namespace SoulEditor
{
    void SoulEditor::StartupWindow::OnInitialize()
    {
        GuiWindow::OnInitialize();
        
    }

    void SoulEditor::StartupWindow::OnUpdate(float deltaTime)
    {
        GuiWindow::OnUpdate(deltaTime);
    }

    void SoulEditor::StartupWindow::OnRender()
    {
        GuiWindow::OnRender();

        // 设置欢迎窗口为全屏覆盖，居中显示
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        // 禁止窗口移动、调整大小、分离等操作的窗口标志
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin("Welcome to SoulEditor", nullptr, window_flags);
        RenderBackground();
        RenderTitle();
        RenderActionButtons();
        HandleFileDialogs();
        ImGui::End();
    }

    void SoulEditor::StartupWindow::OnShutdown()
    {
        GuiWindow::OnShutdown();
    }

    void SoulEditor::StartupWindow::RenderBackground()
    {
        // 设置背景色，营造专业的启动界面感觉
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();

        // 添加渐变背景
        ImU32 bg_color_top = IM_COL32(40, 44, 52, 255); // 深灰蓝色
        ImU32 bg_color_bottom = IM_COL32(25, 28, 34, 255); // 更深的灰色
        draw_list->AddRectFilledMultiColor(window_pos,
                                           ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y),
                                           bg_color_top, bg_color_top, bg_color_bottom, bg_color_bottom);
    }

    void StartupWindow::RenderTitle()
    {
        // 添加一些顶部间距和居中的标题
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50.0f);
        
        // 居中显示标题，使用更大的字体
        float windowWidth = ImGui::GetWindowSize().x;
        const char* title = "Welcome to SoulEditor";
        
        // 使用更大的字体渲染标题
        ImGui::PushFont(ImGui::GetIO().FontDefault); // 可以替换为大字体
        float titleWidth = ImGui::CalcTextSize(title).x;
        ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
        
        // 使用渐变颜色的标题
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "%s", title);
        ImGui::PopFont();
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30.0f);
    }

    void StartupWindow::RenderActionButtons()
    {
        // 居中显示内容区域
        float windowWidth = ImGui::GetWindowSize().x;
        float contentWidth = 500.0f;
        ImGui::SetCursorPosX((windowWidth - contentWidth) * 0.5f);
        
        ImGui::BeginChild("WelcomeContent", ImVec2(contentWidth, 0), false, ImGuiWindowFlags_NoScrollbar);
        
        // 设置一个更柔和的提示文本颜色
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.9f, 1.0f));
        ImGui::TextWrapped("choose an action:");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        // 设置按钮样式
        PushWelcomeStyles();
        
        if (ImGui::Button("Create Project", ImVec2(200, 40))) 
        {
            ImGuiFileDialog::Instance()->OpenDialog("CreateProjectDlg", "Select Project Folder", ".");
        }

        ImGui::SameLine();
        if (ImGui::Button("Open Project", ImVec2(200, 40))) 
        {
            ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", ".json");
        }
        
        PopWelcomeStyles();
        
        ImGui::EndChild();
    }

    void StartupWindow::PushWelcomeStyles()
    {
        // 设置按钮样式
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.8f));         // 蓝色按钮
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));  // 悬停时更亮
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.53f, 0.98f, 1.0f));   // 点击时更深
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);        
    }

    void StartupWindow::PopWelcomeStyles()
    {
        ImGui::PopStyleVar(1);      // 弹出样式变量
        ImGui::PopStyleColor(3);   // 弹出颜色设置
    }

    void StartupWindow::HandleFileDialogs()
    {
        HandleCreateProjectDialog();
        HandleOpenProjectDialog();
    }

    void StartupWindow::HandleCreateProjectDialog()
    {
        // 文件对话框的通用设置
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, 
                              viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
        ImVec2 dialogSize = ImVec2(800, 600);
        
        // Handle CreateProject dialog result - 限制对话框的移动和缩放，并居中显示
        ImGui::SetNextWindowPos(ImVec2(center.x - dialogSize.x * 0.5f, center.y - dialogSize.y * 0.5f), ImGuiCond_Appearing);
        
        if (ImGuiFileDialog::Instance()->Display("CreateProjectDlg", 
                                                ImGuiWindowFlags_NoResize | 
                                                ImGuiWindowFlags_NoMove |
                                                ImGuiWindowFlags_NoCollapse,
                                                dialogSize, // 固定大小
                                                dialogSize)) 
        {
            if (ImGuiFileDialog::Instance()->IsOk()) 
            {
                std::string folder=ImGuiFileDialog::Instance()->GetFilePathName();
                SoulEngine::Logger::Log("Selected path for new project: {}", folder);
                if (folder.empty())
                {
                    SoulEngine::Logger::Warn("No folder selected for new project.");
                    folder = ImGuiFileDialog::Instance()->GetCurrentPath();
                }
                auto [path,name] = SoulEngine::FileUtil::ExtractDirectory(folder);
                ProjectManager::GetInstance().CreateProject(path,name);
                
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void StartupWindow::HandleOpenProjectDialog()
    {
        // 文件对话框的通用设置
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, 
                              viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
        ImVec2 dialogSize = ImVec2(800, 600);
        
        // Handle OpenProject dialog result - 限制对话框的移动和缩放，并居中显示
        ImGui::SetNextWindowPos(ImVec2(center.x - dialogSize.x * 0.5f, center.y - dialogSize.y * 0.5f), ImGuiCond_Appearing);
        
        if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg", 
                                                ImGuiWindowFlags_NoResize | 
                                                ImGuiWindowFlags_NoMove |
                                                ImGuiWindowFlags_NoCollapse,
                                                dialogSize, // 固定大小
                                                dialogSize)) 
        {
            if (ImGuiFileDialog::Instance()->IsOk()) 
            {
                std::string folder=ImGuiFileDialog::Instance()->GetFilePathName();
                SoulEngine::Logger::Log("Selected path for new project: {}", folder);
                if (folder.empty())
                {
                    SoulEngine::Logger::Warn("No folder selected for new project.");
                    folder = ImGuiFileDialog::Instance()->GetCurrentPath();
                }
                ProjectManager::GetInstance().OpenProject(folder);
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }
}
