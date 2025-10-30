#pragma once
#include "Define.h"
#include <string>
#include "Project.h"
#include <functional>
namespace SoulEditor
{
    class ProjectManager
    {
        SINGLETON_CLASS(ProjectManager)
    public:
        bool CreateProject(const std::string& path,const std::string& name);
        bool OpenProject(const std::string& path);
        void SetOnProjectOpenedCallback(const std::function<void()>& callback);

        void SetOnProjectCreatedCallback(const std::function<void()>& callback);
        std::shared_ptr<Project> GetCurrentProject();
        std::vector<std::string> GetRecentProjects();
        void RemoveRecentProject(const std::string& path);
        void ClearRecentProjects();

    private:
        void AddToRecentProjects(const std::string& path);
    private:
        std::function<void()> onProjectOpenedCallback_;
        std::function<void()> onProjectCreatedCallback_;
        std::shared_ptr<Project> currentProject_;
    };

}
