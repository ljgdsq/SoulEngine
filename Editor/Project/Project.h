#pragma once
#include <string>
#include <filesystem>
#include "Define.h"

namespace SoulEditor
{
    /**
     * @brief 简单的项目设置结构
     */
    struct ProjectSettings
    {
        std::string name = "Unnamed Project";           // 项目名称
        std::string version = "1.0.0";        // 项目版本
        std::string description = "No Description";    // 项目描述

        ProjectSettings() {}
    };

    /**
     * @brief 项目管理类 - 处理项目的创建、加载和保存
     */
    class Project
    {
        NON_COPY_AND_MOVE(Project);

    public:
        Project() = default;
        ~Project() = default;

        /**
         * @brief 创建新项目
         * @param path 项目保存路径
         * @param name 项目名称
         * @return true if successful
         */
        bool CreateNewProject(const std::string &path, const std::string &name);

        /**
         * @brief 加载现有项目
         * @param path 项目文件路径
         * @return true if successful
         */
        bool LoadProject(const std::string &path);

        /**
         * @brief 保存当前项目
         * @return true if successful
         */
        bool SaveProject();

        /**
         * @brief 关闭项目
         */
        void CloseProject();

        // 获取器
        const std::string &GetProjectPath() const { return projectPath_; }
        const std::string &GetProjectName() const { return settings_.name; }
        const ProjectSettings &GetSettings() const { return settings_; }
        
        // 设置器
        void SetProjectName(const std::string &name) { settings_.name = name; isDirty_ = true; }
        void SetProjectDescription(const std::string &desc) { settings_.description = desc; isDirty_ = true; }
        
        // 状态查询
        bool IsLoaded() const { return isLoaded_; }
        bool IsDirty() const { return isDirty_; }
        
        // 项目目录结构
        std::string GetAssetsPath() const { return projectPath_ + "/Assets"; }
        std::string GetLibraryPath() const { return projectPath_ + "/Library"; }
        std::string GetLogsPath() const { return projectPath_ + "/Logs"; }

    private:
        std::string projectPath_;       // 项目根目录路径
        ProjectSettings settings_;      // 项目设置
        bool isLoaded_ = false;        // 是否已加载项目
        bool isDirty_ = false;         // 是否有未保存的更改
        
        // 私有辅助方法
        bool CreateProjectDirectories() const;
        bool CreateProjectFile();
        bool LoadProjectFile();
        bool SaveProjectFile();
        void MarkClean() { isDirty_ = false; }
    };

} // namespace SoulEngine