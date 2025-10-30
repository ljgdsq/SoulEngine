#include "ProjectManager.h"

#include "EditorPrefs.h"
#include "EngineFileIO.h"
#include "Logger.h"

namespace SoulEditor
{
    bool ProjectManager::CreateProject(const std::string& path, const std::string& name)
    {
        auto proj = std::make_shared<Project>();
        proj->CreateNewProject(path, name);
        currentProject_ = proj;
        SoulEngine::EngineFileIO::SetProjectPath(proj->GetProjectPath());
        AddToRecentProjects(proj->GetProjectPath());
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
        AddToRecentProjects(proj->GetProjectPath());
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

    std::vector<std::string> ProjectManager::GetRecentProjects()
    {
        auto content=EditorPrefs::GetInstance().GetString("RecentProjects","");
        if (content.empty() || content=="[]")
        {
            return {};
        }

        try
        {   
            auto json=nlohmann::json::parse(content);
            if (json.is_array())
            {
                return json.get<std::vector<std::string>>();
            }
        }
        catch (...)
        {
            SoulEngine::Logger::Error("GetRecentProjects failed to parse recent projects from EditorPrefs.");
        }
        return {};
    }

    void ProjectManager::RemoveRecentProject(const std::string& path)
    {
        auto recentProjects = GetRecentProjects();
        auto it = std::find(recentProjects.begin(), recentProjects.end(), path);
        if (it != recentProjects.end())
        {
            recentProjects.erase(it);
            // 保存回去
            nlohmann::json jsonArray = recentProjects;
            EditorPrefs::GetInstance().SetString("RecentProjects", jsonArray.dump());
            EditorPrefs::GetInstance().Save();
        }
    }

    void ProjectManager::ClearRecentProjects()
    {
        EditorPrefs::GetInstance().DeleteKey("RecentProjects");
        EditorPrefs::GetInstance().Save();
    }

    void ProjectManager::AddToRecentProjects(const std::string& projectPath)
    {
        auto recentProjects = GetRecentProjects();
    
        // 移除已存在的项目（如果有）
        auto it = std::find(recentProjects.begin(), recentProjects.end(), projectPath);
        if (it != recentProjects.end())
        {
            recentProjects.erase(it);
        }
    
        // 添加到开头
        recentProjects.insert(recentProjects.begin(), projectPath);
    
        // 限制最大数量（比如10个）
        if (recentProjects.size() > 10)
        {
            recentProjects.resize(10);
        }
    
        // 保存回去
        nlohmann::json jsonArray = recentProjects;
        EditorPrefs::GetInstance().SetString("RecentProjects", jsonArray.dump());
        EditorPrefs::GetInstance().Save();
    }
}
