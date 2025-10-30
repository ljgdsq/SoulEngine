#include "Project.h"
#include <filesystem>
#include "nlohmann/json.hpp"
#include "Core/EngineFileIO.h"
using SoulEngine::EngineFileIO;
namespace SoulEditor
{
    bool Project::CreateNewProject(const std::string &path, const std::string &name)
    {
        // 设置项目基本信息
        projectPath_ = (std::filesystem::path(path) / name).string();
        settings_.name = name;
        settings_.description = "";
        
        // 创建项目目录结构
        if (!CreateProjectDirectories())
        {
            return false;
        }
        
        // 创建项目文件
        if (!CreateProjectFile())
        {
            return false;
        }
        
        isLoaded_ = true;
        isDirty_ = false;
        return true;
    }

    bool Project::LoadProject(const std::string &path)
    {
        // 检查路径是否为项目文件还是项目目录
        std::filesystem::path fsPath(path);
        
        if (std::filesystem::is_directory(fsPath))
        {
            // 如果是目录，查找项目文件
            projectPath_ = path;
            std::filesystem::path projectFile = fsPath / "project.json";
            if (!std::filesystem::exists(projectFile))
            {
                return false;
            }
        }
        else if (std::filesystem::is_regular_file(fsPath))
        {
            // 如果是文件，获取父目录作为项目路径
            projectPath_ = fsPath.parent_path().string();
        }
        else
        {
            return false;
        }
        
        // 加载项目文件
        if (!LoadProjectFile())
        {
            return false;
        }
        
        isLoaded_ = true;
        isDirty_ = false;
        return true;
    }

    bool Project::SaveProject()
    {
        if (!isLoaded_)
        {
            return false;
        }
        
        if (SaveProjectFile())
        {
            MarkClean();
            return true;
        }
        
        return false;
    }

    void Project::CloseProject()
    {
        projectPath_.clear();
        settings_ = ProjectSettings();
        isLoaded_ = false;
        isDirty_ = false;
    }

    bool Project::CreateProjectDirectories() const
    {
        try
        {
            // 创建主要目录
            std::filesystem::create_directories(projectPath_);
            std::filesystem::create_directories(GetAssetsPath());
            std::filesystem::create_directories(GetLibraryPath());
            std::filesystem::create_directories(GetLogsPath());
            return true;
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return false;
        }
    }

    bool Project::CreateProjectFile()
    {
        return SaveProjectFile();
    }

    bool Project::LoadProjectFile()
    {
        std::filesystem::path projectFile = std::filesystem::path(projectPath_) / "project.json";
        auto content = EngineFileIO::LoadText(projectFile.string());
        auto json = nlohmann::json::parse(content);
        settings_.name = json.value("name", "Untitled");
        settings_.version = json.value("version", "1.0");
        settings_.description = json.value("description", "");
        return true;
    }

    bool Project::SaveProjectFile()
    {
        std::filesystem::path projectFile = std::filesystem::path(projectPath_) / "project.json";
        nlohmann::json json;
        json["name"] = settings_.name;
        json["version"] = settings_.version;
        json["description"] = settings_.description;
        EngineFileIO::SaveText(projectFile.string(),json.dump(4));
        return true;
    }

} // namespace SoulEngine