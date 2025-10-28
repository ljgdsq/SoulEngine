#include "MainEditorUI.h"
#include "ProjectManager.h"
#include "ImGuiFileDialog.h"
#include "GLFW/glfw3.h"

namespace SoulEditor
{
    MainEditorUI::MainEditorUI()
    {
        // 构造函数中可以进行初始化设置
    }
    
    void MainEditorUI::Render()
    {
        RenderMainMenuBar();
        RenderProjectBrowser();
        RenderSceneHierarchy();
        RenderInspector();
        RenderConsole();
        HandleFileDialogs();
    }
    
    void MainEditorUI::ShowPanel(const std::string& panelName, bool show)
    {
        if (panelName == "Project Browser")
            showProjectBrowser_ = show;
        else if (panelName == "Scene Hierarchy")
            showSceneHierarchy_ = show;
        else if (panelName == "Inspector")
            showInspector_ = show;
        else if (panelName == "Console")
            showConsole_ = show;
    }
    
    bool MainEditorUI::IsPanelVisible(const std::string& panelName) const
    {
        if (panelName == "Project Browser")
            return showProjectBrowser_;
        else if (panelName == "Scene Hierarchy")
            return showSceneHierarchy_;
        else if (panelName == "Inspector")
            return showInspector_;
        else if (panelName == "Console")
            return showConsole_;
        return false;
    }
    
    void MainEditorUI::ResetLayout()
    {
        showProjectBrowser_ = true;
        showSceneHierarchy_ = true;
        showInspector_ = true;
        showConsole_ = true;
    }
    
    void MainEditorUI::RenderMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar()) 
        {
            HandleFileMenu();
            HandleViewMenu();
            HandleHelpMenu();
            ImGui::EndMainMenuBar();
        }
    }
    
    void MainEditorUI::HandleFileMenu()
    {
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("New Project")) 
            {
                if (onCloseProject_)
                    onCloseProject_();
            }
            if (ImGui::MenuItem("Open Project")) 
            {
                ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) 
            {
                // TODO: 实现保存场景功能
            }
            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) 
            {
                // TODO: 实现另存为场景功能
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) 
            {
                // 设置应用退出标志
                glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
    }
    
    void MainEditorUI::HandleViewMenu()
    {
        if (ImGui::BeginMenu("View")) 
        {
            ImGui::MenuItem("Project Browser", nullptr, &showProjectBrowser_);
            ImGui::MenuItem("Scene Hierarchy", nullptr, &showSceneHierarchy_);
            ImGui::MenuItem("Inspector", nullptr, &showInspector_);
            ImGui::MenuItem("Console", nullptr, &showConsole_);
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) 
            {
                ResetLayout();
            }
            ImGui::EndMenu();
        }
    }
    
    void MainEditorUI::HandleHelpMenu()
    {
        if (ImGui::BeginMenu("Help")) 
        {
            if (ImGui::MenuItem("Documentation")) 
            {
                // TODO: 打开文档链接
            }
            if (ImGui::MenuItem("Report Bug")) 
            {
                // TODO: 打开bug报告链接
            }
            ImGui::Separator();
            if (ImGui::MenuItem("About SoulEditor")) 
            {
                // TODO: 显示关于对话框
            }
            ImGui::EndMenu();
        }
    }
    
    void MainEditorUI::RenderProjectBrowser()
    {
        if (showProjectBrowser_) 
        {
            ImGui::Begin("Project Browser", &showProjectBrowser_);
            
            const auto& projectRoot = ProjectManager::GetInstance().GetCurrentProjectRoot();
            ImGui::Text("Current Project: %s", projectRoot.c_str());
            
            ImGui::Separator();
            
            // TODO: 实现项目文件浏览功能
            ImGui::Text("Project files will be displayed here");
            
            ImGui::Spacing();
            
            // 添加关闭项目按钮
            if (ImGui::Button("Close Project")) 
            {
                if (onCloseProject_)
                    onCloseProject_();
            }
            
            ImGui::End();
        }
    }
    
    void MainEditorUI::RenderSceneHierarchy()
    {
        if (showSceneHierarchy_) 
        {
            ImGui::Begin("Scene Hierarchy", &showSceneHierarchy_);
            
            // TODO: 实现场景层级显示功能
            ImGui::Text("Scene objects will be listed here");
            
            // 示例场景对象
            if (ImGui::TreeNode("Scene Root"))
            {
                if (ImGui::TreeNode("GameObject 1"))
                {
                    ImGui::Text("Component: Transform");
                    ImGui::Text("Component: MeshRenderer");
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("GameObject 2"))
                {
                    ImGui::Text("Component: Transform");
                    ImGui::Text("Component: Camera");
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            
            ImGui::End();
        }
    }
    
    void MainEditorUI::RenderInspector()
    {
        if (showInspector_) 
        {
            ImGui::Begin("Inspector", &showInspector_);
            
            // TODO: 实现属性检查器功能
            ImGui::Text("Object properties will be shown here");
            
            // 示例属性编辑
            ImGui::Separator();
            ImGui::Text("Transform");
            
            static float position[3] = {0.0f, 0.0f, 0.0f};
            static float rotation[3] = {0.0f, 0.0f, 0.0f};
            static float scale[3] = {1.0f, 1.0f, 1.0f};
            
            ImGui::DragFloat3("Position", position, 0.1f);
            ImGui::DragFloat3("Rotation", rotation, 1.0f);
            ImGui::DragFloat3("Scale", scale, 0.1f);
            
            ImGui::End();
        }
    }
    
    void MainEditorUI::RenderConsole()
    {
        if (showConsole_) 
        {
            ImGui::Begin("Console", &showConsole_);
            
            // TODO: 实现控制台日志显示功能
            ImGui::Text("Engine logs and debug information");
            
            // 示例日志条目
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[INFO] SoulEditor started successfully");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[WARNING] Texture not found: default.png");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[ERROR] Failed to load shader: invalid_shader.hlsl");
            
            ImGui::End();
        }
    }
    
    void MainEditorUI::HandleFileDialogs()
    {
        // 处理文件对话框（在主编辑器中可以允许正常的拖拽缩放）
        if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg")) 
        {
            if (ImGuiFileDialog::Instance()->IsOk()) 
            {
                std::string sel = ImGuiFileDialog::Instance()->GetFilePathName();
                if (sel.empty()) sel = ImGuiFileDialog::Instance()->GetCurrentPath();
                if (!sel.empty()) 
                {
                    ProjectManager::GetInstance().OpenProject(sel);
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }
}