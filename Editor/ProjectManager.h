#pragma once

#include <string>
#include <vector>

namespace SoulEditor
{
    /**
     * @brief 项目管理器 - 负责项目的创建、打开、管理
     */
    class ProjectManager
    {
    public:
        static ProjectManager& GetInstance();
        
        // 项目操作
        bool CreateProject(const std::string& projectPath);
        bool OpenProject(const std::string& projectPath);
        void CloseProject();
        
        // 项目状态查询
        bool IsProjectOpened() const { return isProjectOpened_; }
        const std::string& GetCurrentProjectRoot() const { return currentProjectRoot_; }
        
        // 最近项目管理
        const std::vector<std::string>& GetRecentProjects() const { return recentProjects_; }
        void AddToRecentProjects(const std::string& path);
        void LoadRecentProjects();
        void SaveRecentProjects();
        
        // 事件回调
        void SetOnProjectOpenedCallback(std::function<void(const std::string&)> callback) { 
            onProjectOpened_ = callback; 
        }
        void SetOnProjectClosedCallback(std::function<void()> callback) { 
            onProjectClosed_ = callback; 
        }
        
    private:
        ProjectManager() = default;
        ~ProjectManager() = default;
        ProjectManager(const ProjectManager&) = delete;
        ProjectManager& operator=(const ProjectManager&) = delete;
        
        // 项目状态
        bool isProjectOpened_ = false;
        std::string currentProjectRoot_;
        std::vector<std::string> recentProjects_;
        
        // 事件回调
        std::function<void(const std::string&)> onProjectOpened_;
        std::function<void()> onProjectClosed_;
        
        // 内部方法
        bool CreateProjectStructure(const std::string& root);
        std::string ResolveProjectPath(const std::string& pathOrFile);
    };
}