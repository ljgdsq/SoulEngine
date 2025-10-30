#include "ProjectManager.h"

#include "EngineFileIO.h"

namespace SoulEditor
{
    bool ProjectManager::CreateProject(const std::string& path, const std::string& name)
    {
        auto proj = std::make_shared<Project>();
        proj->CreateNewProject(path, name);
        currentProject_ = proj;
        SoulEngine::EngineFileIO::SetProjectPath(proj->GetProjectPath());
        if (onProjectCreatedCallback_)
        {
            onProjectCreatedCallback_();
        }
        return true;
    }

    bool ProjectManager::OpenProject(const std::string& path)
    {
        auto proj = std::make_shared<Project>();
        proj->LoadProject(path);
        SoulEngine::EngineFileIO::SetProjectPath(proj->GetProjectPath());
        currentProject_ = proj;
        if (onProjectOpenedCallback_)
        {
            onProjectOpenedCallback_();
        }
        return true;
    }

    void ProjectManager::SetOnProjectOpenedCallback(const std::function<void()>& callback)
    {
        onProjectOpenedCallback_ = callback;
    }

    void ProjectManager::SetOnProjectCreatedCallback(const std::function<void()>& callback)
    {
        onProjectCreatedCallback_ = callback;
    }

    std::shared_ptr<Project> ProjectManager::GetCurrentProject()
    {
        return currentProject_;
    }
}
