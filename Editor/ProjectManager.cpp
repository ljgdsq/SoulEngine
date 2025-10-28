#include "ProjectManager.h"
#include "SoulEngine.h"
#include <filesystem>
#include <fstream>

namespace SoulEditor
{
    ProjectManager& ProjectManager::GetInstance()
    {
        static ProjectManager instance;
        return instance;
    }
    
    bool ProjectManager::CreateProject(const std::string& projectPath)
    {
        try
        {
            if (CreateProjectStructure(projectPath))
            {
                currentProjectRoot_ = projectPath;
                isProjectOpened_ = true;
                AddToRecentProjects(projectPath);
                
                SoulEngine::Logger::Log("Created project at {}", projectPath);
                
                if (onProjectOpened_)
                    onProjectOpened_(projectPath);
                    
                return true;
            }
        }
        catch (const std::exception& e)
        {
            SoulEngine::Logger::Error("Failed to create project at {}: {}", projectPath, e.what());
        }
        
        return false;
    }
    
    bool ProjectManager::OpenProject(const std::string& pathOrFile)
    {
        std::string projectRoot = ResolveProjectPath(pathOrFile);
        
        if (!projectRoot.empty())
        {
            currentProjectRoot_ = projectRoot;
            isProjectOpened_ = true;
            AddToRecentProjects(projectRoot);
            
            SoulEngine::Logger::Log("Opened project: {}", projectRoot);
            
            if (onProjectOpened_)
                onProjectOpened_(projectRoot);
                
            return true;
        }
        else
        {
            SoulEngine::Logger::Error("Invalid project path: {}", pathOrFile);
            return false;
        }
    }
    
    void ProjectManager::CloseProject()
    {
        if (isProjectOpened_)
        {
            SoulEngine::Logger::Log("Closing project: {}", currentProjectRoot_);
            
            isProjectOpened_ = false;
            currentProjectRoot_.clear();
            
            if (onProjectClosed_)
                onProjectClosed_();
        }
    }
    
    void ProjectManager::AddToRecentProjects(const std::string& path)
    {
        // 保持唯一性，最近使用的排在前面，限制为10个
        recentProjects_.erase(
            std::remove(recentProjects_.begin(), recentProjects_.end(), path), 
            recentProjects_.end()
        );
        recentProjects_.insert(recentProjects_.begin(), path);
        
        if (recentProjects_.size() > 10)
            recentProjects_.resize(10);
            
        SaveRecentProjects();
    }
    
    void ProjectManager::LoadRecentProjects()
    {
        // TODO: 从配置文件加载最近项目列表
        // 这里可以实现从 .soulengine/recent_projects.json 加载
    }
    
    void ProjectManager::SaveRecentProjects()
    {
        // TODO: 保存最近项目列表到配置文件
        // 这里可以实现保存到 .soulengine/recent_projects.json
    }
    
    bool ProjectManager::CreateProjectStructure(const std::string& root)
    {
        namespace fs = std::filesystem;
        
        try
        {
            // 创建项目目录结构
            fs::create_directories(fs::path(root) / "Assets");
            fs::create_directories(fs::path(root) / "Library");
            fs::create_directories(fs::path(root) / "StreamingAssets");
            
            // 创建项目配置文件
            fs::path projFile = fs::path(root) / "project.json";
            std::ofstream ofs(projFile);
            if (ofs)
            {
                ofs << "{\n";
                ofs << "  \"name\": \"" << fs::path(root).filename().string() << "\",\n";
                ofs << "  \"version\": \"1.0.0\",\n";
                ofs << "  \"assets\": \"Assets\",\n";
                ofs << "  \"library\": \"Library\",\n";
                ofs << "  \"streamingAssets\": \"StreamingAssets\"\n";
                ofs << "}";
                ofs.close();
                return true;
            }
        }
        catch (const std::exception& e)
        {
            SoulEngine::Logger::Error("Failed to create project structure: {}", e.what());
        }
        
        return false;
    }
    
    std::string ProjectManager::ResolveProjectPath(const std::string& pathOrFile)
    {
        namespace fs = std::filesystem;
        fs::path p(pathOrFile);
        
        if (fs::is_regular_file(p) && p.filename() == "project.json")
        {
            return p.parent_path().string();
        }
        else if (fs::is_directory(p))
        {
            return p.string();
        }
        else if (fs::is_regular_file(p))
        {
            // 如果用户选择了其他文件，使用其父目录作为项目根目录
            return p.parent_path().string();
        }
        
        return "";
    }
}